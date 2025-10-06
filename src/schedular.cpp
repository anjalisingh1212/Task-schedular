#include <algorithm>
#include "scheduler.hpp"

Scheduler::Scheduler(int numThreads) : running(true){
    for(int i = 0; i < numThreads; i++){
        workers.emplace_back(&Scheduler::workerThread, this);
    }
}

Scheduler::~Scheduler(){
    stop();
}

void Scheduler::enqueueTask(const Task &task){
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }
    cv.notify_one();
}

void Scheduler::workerThread(){
    while(running){
        std::unique_lock<std::mutex> lock(queueMutex);
        cv.wait(lock, [&] { return !taskQueue.empty() || !running; });

        if (!running && taskQueue.empty()) break;
        Task task = taskQueue.front();
        taskQueue.pop();
        lock.unlock();

        // Simulate task execution
        std::string result;
        switch(task.getTaskType()){
            case 1: // Add
            {
                int sum = 0;
                for(auto &op : task.getOperands())
                    sum += op;
                result = "Sum = " + std::to_string(sum);
                task.setResult(result);
                break;
            }
            case 2: // Subtract
            {   
                const auto &op = task.getOperands();
                int diff = op[0];

                for(size_t i = 1; i < op.size(); i++)
                    diff -= op[i];
                result = "Difference = " + std::to_string(diff);
                task.setResult(result);
                break;
            }case 3: // Multiply
            {
                int prod = 1;
                for(auto &op : task.getOperands())
                    prod *= op;
                result = "Product " + std::to_string(prod);
                task.setResult(result);
                break;
            }case 4: // Sort numbers
            {   
                auto nums = task.getOperands();
                std::sort(nums.begin(), nums.end());
                result = "Sorted = ";
                for(auto n: nums)
                    result += std::to_string(n) + " ";
                task.setResult(result);
                break;
            }case 5: // Reverse string
            {   
                std::string str = task.getStringData();
                std::reverse(str.begin(), str.end());
                result = "Reversed = " + str;
                task.setResult(result);
                break;
            }
        }

    }
}

void Scheduler::stop(){
    running = false;
    cv.notify_all();
    for (auto &t : workers)
        if (t.joinable()) t.join();
}