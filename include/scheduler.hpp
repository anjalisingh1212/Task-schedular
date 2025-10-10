#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <atomic>
#include "task.hpp"

class Scheduler{
private:
    std::queue <std::shared_ptr<Task>> taskQueue;
    std::queue <std::shared_ptr<Task>> taskResultQueue;
    std::mutex queueMutex;
    std::mutex resultMutex;
    std::condition_variable cv;
    std::condition_variable resultCv;
    std::vector<std::thread> workers;
    std::thread dispatcherThread;
    std::atomic<bool> running;


public:
    Scheduler(int numThreads);
    ~Scheduler();
    void enqueueTask(std::shared_ptr<Task>);
    void workerThread();
    void resultDispatcher();
    void stop();
};

#endif //SCHEDULER_HPP