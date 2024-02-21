
// #pragma once
#ifndef IMGUI_THREAD_H
#define IMGUI_THREAD_H

#include <mutex>
#include <condition_variable>
#include <queue>

// Define a message structure
struct Message {
  unsigned int endlessCalcThreads; // amount of special threads dedicated to endless calculations
  unsigned int allocMbytes;        // amount of RAM to be allocated
  unsigned int numPiCalcTasks;     // number of simple PI calculation tasks to be started. Once they finish, they stop.
  unsigned int triggerPiTasks;     // single trigger to calc PI.
  bool quitFlag;
};

// Define a message structure
struct MessageCntrl_s {
    // Shared state between threads
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<Message> messageQueue;
};

void ImGuiSendMessage(MessageCntrl_s& msgCtrl, Message message);
Message MainThreadReceiveMessage(MessageCntrl_s& msgCtrl);
int imguiTh(MessageCntrl_s& msgCtl);
#endif // #ifndef IMGUI_THREAD_H