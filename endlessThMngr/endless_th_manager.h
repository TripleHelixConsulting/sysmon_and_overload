#ifndef ENDLESS_THREAD_H
#define ENDLESS_THREAD_H

#include <thread>
#include <atomic>
#include <string>
#include <vector>

class EndlessThread {
public:
    EndlessThread();

    void Start();
    void Terminate();
    void Join();

private:
    std::thread thread_;
    std::atomic<bool> running_;
    std::atomic<bool> termination_flag_;
};

class EndlessThreadManager {
public:
    EndlessThreadManager();

    void setNumberOfEndlessThreads(unsigned int numThreads);

private:
    std::vector<std::unique_ptr<EndlessThread>> threads_;
    const unsigned int max_threads_;
};

#endif // #ifndef ENDLESS_THREAD_H