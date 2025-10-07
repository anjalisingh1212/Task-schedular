#include <algorithm>
#include <cstring>
#include <mqueue.h>
#include "scheduler.hpp"

Scheduler::Scheduler(int numThreads) : running(true){
    for(int i = 0; i < numThreads; i++){
        workers.emplace_back(&Scheduler::workerThread, this);
    }
}

Scheduler::~Scheduler(){
    stop();
}

void Scheduler::enqueueTask(std::shared_ptr<Task> task){
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }
    cv.notify_one();
}

void Scheduler::workerThread(){
    while(running){
        std::unique_lock<std::mutex> lock(queueMutex);
        // Wait until there is a task to send or the scheduler is stopping
        cv.wait(lock, [&] { return !taskQueue.empty() || !running; });

        if (!running && taskQueue.empty()) break;
        std::shared_ptr<Task> task = taskQueue.front();
        taskQueue.pop();
        lock.unlock();

        // Simulate task execution
        std::string result;
        switch(task->getTaskType()){
            case 1: // Add
            {
                int sum = 0;
                for(auto &op : task->getOperands())
                    sum += op;
                result = "Sum = " + std::to_string(sum);
                break;
            }
            case 2: // Subtract
            {   
                const auto &op = task->getOperands();
                int diff = op[0];

                for(size_t i = 1; i < op.size(); i++)
                    diff -= op[i];
                result = "Difference = " + std::to_string(diff);
                break;
            }case 3: // Multiply
            {
                int prod = 1;
                for(auto &op : task->getOperands())
                    prod *= op;
                result = "Product " + std::to_string(prod);
                break;
            }case 4: // Sort numbers
            {   
                auto nums = task->getOperands();
                std::sort(nums.begin(), nums.end());
                result = "Sorted = ";
                for(auto n: nums)
                    result += std::to_string(n) + " ";
                break;
            }case 5: // Reverse string
            {   
                std::string str = task->getStringData();
                std::reverse(str.begin(), str.end());
                result = "Reversed = " + str;
                break;
            }
        }
        
        // Save result inside task object
        task->setResult(result);

        // Push completed task into result queue
        {
            std::lock_guard<std::mutex> resLock(resultMutex); //consturctor call for lock_guard
            taskResultQueue.emplace(task);
        }

        resultCv.notify_one();
    }
}

void Scheduler::resultDispatcher(){
    std::shared_ptr<Task> task;
    while(running){
        std::unique_lock<std::mutex> lock(resultMutex);
        // Wait until there is a task to send or the scheduler is stopping
        resultCv.wait(lock, [&]{return !taskResultQueue.empty() || !running; });
        if(!running && taskResultQueue.empty())
            break;
        std::shared_ptr<Task> task = taskResultQueue.front();
        taskResultQueue.pop();
    }

    char resultMsg[MAX_RESULT_MSG_SIZE];
    snprintf(resultMsg, MAX_RESULT_MSG_SIZE, 
            "Task %d Result %s",
            task->getTaskId(), task->getResult().c_str());
    
    mqd_t clientMq = mq_open(task->getMqName().c_str(), O_WRONLY);
    size_t bytes_send = mq_send(clientMq, resultMsg, strlen(resultMsg)+1, 0);
    if(bytes_send  == -1){
        perror("mq_send failed");
    }else {
            std::cout << "[Dispatcher] Sent result for Task " << task->getTaskId()
                      << " to " << task->getMqName() << std::endl;
        }

    mq_close(clientMq);
}
void Scheduler::stop(){
    running = false;
    cv.notify_all();
    for (auto &t : workers)
        if (t.joinable()) t.join();
}