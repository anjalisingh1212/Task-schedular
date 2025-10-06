#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <atomic>
#include "task.hpp"

class Scheduler{
private:
    std::queue<Task> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::vector<std::thread> workers;
    std::atomic<bool> running;


public:
    Scheduler(int numThreads);
    ~Scheduler();
    void enqueueTask(const Task &task);
    void workerThread();
    void stop();
};

#endif //SCHEDULER_HPP