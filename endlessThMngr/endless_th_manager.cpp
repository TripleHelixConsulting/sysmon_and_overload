#include "endless_th_manager.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <chrono>
#include <thread>
#include <cmath>
#include <atomic>
#include <vector>

  EndlessThread::EndlessThread() : running_(true) {}

volatile double pi_vol_e;

  void calculate_PI_15th_sign_et(double add){
      int decimal_places = 15;
      double pi = 0.0;
      bool sign = true;
      for (int i = 0; i <= decimal_places; ++i) {
           double term = pow(-1.0, i) / (2.0 * i + 1);
           pi += sign * term;
           sign = !sign;
      }
      pi_vol_e = pi+add;
  }

  void calc_100000_times_et(){
      for (unsigned int i = 0; i <= 100000; ++i) {
          calculate_PI_15th_sign_et(1.0);
      }
  }

  void EndlessThread::Start() {
    thread_ = std::thread([this]() {
      while (running_.load()) {
        // Your thread logic here
        // std::cout << "Endless thread working..." << std::endl;
        calc_100000_times_et();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));  // Example sleep

        // Check for termination message periodically
        if (termination_flag_) {
          std::cout << "Killing the thread " << std::endl;
          running_.store(false);
        }
      }
    });
  }

  void EndlessThread::Terminate() {
    termination_flag_.store(true);
  }

  void EndlessThread::Join() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }


  EndlessThreadManager::EndlessThreadManager() : max_threads_(1000) {}

  void EndlessThreadManager::setNumberOfEndlessThreads(unsigned int numThreads) {
    if (numThreads > max_threads_) {
      numThreads = max_threads_; // Enforce maximum
    }

    while (threads_.size() < numThreads) {
      threads_.emplace_back(std::make_unique<EndlessThread>());
      threads_.back()->Start();
    }

    while (threads_.size() > numThreads) {
      threads_.back()->Terminate();
      threads_.back()->Join();
      threads_.pop_back();
    }
  }