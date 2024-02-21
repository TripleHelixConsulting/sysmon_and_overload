/***********************************************************************************************************************
* @file main.cpp
*
* @brief Test code for the C++11 Thread Pool of Atanas Rusev
*
* @details	 
*
*
*  The code is based completely on C++11 features. The purpose is to be able to integrate it
*  in older projects which have not yet reached C++14 or higher. If you need newer features
*  fork the code and get it to the next level yourself.
*
* @author Atanas Rusev
*
* @copyright 2023 Atanas Rusev, GPL License. Check the License file in the library.
*
***********************************************************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include <atomic>
#include <signal.h>
#include "thread_pool/thread_pool.h"
#include "include/imgui_thread.h"
#include "endlessThMngr/endless_th_manager.h"

using namespace std::chrono_literals;
using namespace std;

/***********************************************************************************************************************
* @brief Main that creates a thread pool and tests it.
*
* @details	Tested with default number of threads (the return of std::thread::hardware_concurrency). Tested also 
* with 1, 4 and 40 threads. Test Machine - HP ZBook 15 G5 with 16 GB RAM and Intel� Core� i7-8750H Processor.
* Built initially with Microsoft Visual Studio 2015 as a console application.
*
* @author Atanas Rusev
*
* @copyright 2023 Atanas Rusev, GPL License. Check the License file in the library.
*
***********************************************************************************************************************/

volatile double pi_vol;

void calculate_PI_15th_sign(double add){
     int decimal_places = 15;
     double pi = 0.0;
     bool sign = true;
     for (int i = 0; i <= decimal_places; ++i) {
          double term = pow(-1.0, i) / (2.0 * i + 1);
          pi += sign * term;
          sign = !sign;
     }
     pi_vol = pi+add;
}

void calc_1000000_times(){
      for (unsigned int i = 0; i <= 1000000; ++i) {
          calculate_PI_15th_sign(1.0);
      }
}

volatile bool firstFlg = 1;
volatile int debug1MBcntr = 0;
vector<unique_ptr<vector<double>>> allocated_data;

void allocate_and_fill_memory_1MB(void) {
  // Convert megabytes to bytes
  size_t bytes_to_allocate = 1024 * 1024;

  // Attempt to allocate memory
  std::unique_ptr<std::vector<double>> data = std::make_unique<std::vector<double>>(bytes_to_allocate / sizeof(double));

  // Check if allocation was successful
  if (!data) {
    cerr << "Error: Failed to allocate memory for 1 MB vector." << endl;
    return; // Exit the function on failure
  }

  // Fill the vector with values
  for (size_t i = 0; i < data->size(); ++i) {
    (*data)[i] = pi_vol;
  }

  // Add the data to allocated_data
  allocated_data.emplace_back(std::move(data));
  debug1MBcntr++;
}


void remove_and_deallocate_vector(void) {
  // Check if there are any vectors to remove
  if (allocated_data.empty()) {
    cerr << "Warning: No vectors to remove from allocated_data." << endl;
    return; // Exit the function if empty
  }

  // Remove the last vector and deallocate memory
  // allocated_data.pop_back();
  debug1MBcntr--;

  cout << "Removed and deallocated 1 MB of memory." << endl;

}

bool quitMainLoop = false;

int main()
{
	CTP::ThreadPool thread_pool;

     EndlessThreadManager endlessManager;

     MessageCntrl_s MessageCtl;
     Message localCtlMsg = {0,0,0,0, false};
     bool msgChanged = 0;     
     
     allocated_data.reserve(2048);
     
     // here we create the imgui thread.
     std::thread imth(imguiTh, std::ref(MessageCtl));

     while(false == quitMainLoop){
          // Message MainThreadReceiveMessage(MessageCntrl_s& msgCtrl)
          auto msg = MainThreadReceiveMessage(MessageCtl);
          
          if (localCtlMsg.allocMbytes != msg.allocMbytes ||
          localCtlMsg.endlessCalcThreads != msg.endlessCalcThreads ||
          localCtlMsg.numPiCalcTasks != msg.numPiCalcTasks ||
          localCtlMsg.triggerPiTasks != msg.triggerPiTasks ||
          localCtlMsg.quitFlag != msg.quitFlag)
          msgChanged = true;

          
          if (true == firstFlg || true == msgChanged){
               if (true == msg.quitFlag){
                    // deallocate all memory
                    for (unsigned int i = 0; i < localCtlMsg.allocMbytes; i++){
                         remove_and_deallocate_vector();
                    }
                    // kill all Endless threads
                    endlessManager.setNumberOfEndlessThreads(0); 
                    // the thread pool handling is integrated in itself upon destruction!
                    // simply break the loop:
                    quitMainLoop = true;
               } else {
                    // main work - first check allocate bytes
                    if (localCtlMsg.allocMbytes > msg.allocMbytes){
                         // deallocate
                         for (unsigned int i = 0; i < localCtlMsg.allocMbytes - msg.allocMbytes; i++){
                              remove_and_deallocate_vector();
                         }
                    } else {
                         if (localCtlMsg.allocMbytes < msg.allocMbytes){
                              for (unsigned int i = 0; i < msg.allocMbytes - localCtlMsg.allocMbytes; i++){
                                   allocate_and_fill_memory_1MB();
                              }
                         }
                    } // else do nothing - the amount is the same as before.

                    // main work - check endlessCalcThreads
                    if (localCtlMsg.endlessCalcThreads != msg.endlessCalcThreads){
                         endlessManager.setNumberOfEndlessThreads(msg.endlessCalcThreads); 
                    }

                    // main work - check triggerPiTasks
                    if (localCtlMsg.triggerPiTasks != msg.triggerPiTasks){
                         // trigger numPiCalcTasks small calc tasks in separate threads
                         for (unsigned int i = 0; i < localCtlMsg.numPiCalcTasks; i++){
                              thread_pool.Schedule([i]()
                              {
                                   calc_1000000_times();	
                              });
                         }
                    } 

                    // finally simply take the values
                    localCtlMsg.allocMbytes = msg.allocMbytes;
                    localCtlMsg.endlessCalcThreads = msg.endlessCalcThreads;
                    localCtlMsg.numPiCalcTasks = msg.numPiCalcTasks;
                    localCtlMsg.triggerPiTasks = msg.triggerPiTasks;
                    std::cout << "/n" << msg.endlessCalcThreads << endl;
                    std::cout << msg.allocMbytes << endl;
                    std::cout << msg.numPiCalcTasks << endl;
                    std::cout << msg.triggerPiTasks << endl;
                    firstFlg = false;
                    msgChanged = false;
               }               
          }
     }    

     // wait for the thread to end at program exit
     imth.join();

     std::cout << "Main thread finished." << std::endl;

	return 0;
}
