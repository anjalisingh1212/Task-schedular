#include <iostream>
#include <algorithm>
#include <cstring>
#include <mqueue.h>
#include "scheduler.hpp"
#include "ipcConfig.hpp"

Scheduler::Scheduler(int numThreads) : running(true){
    for(int i = 0; i < numThreads; i++){
        workers.emplace_back(&Scheduler::workerThread, this);
    }
    dispatcherThread = std::thread(&Scheduler::dispatcherThread, this);
}

Scheduler::~Scheduler(){
    stop();
}

void Scheduler::enqueueTask(std::shared_ptr<Task> task){
    std::cout << "Task is added in queue. Id : " << task->getTaskId() << std::endl;
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }
    cv.notify_one();
}

void Scheduler::workerThread(){
    std::cout << "Worker thread is started." << std::endl;
    while(running){
        std::unique_lock<std::mutex> lock(queueMutex);
        // Wait until there is a task to send or the scheduler is stopping
        cv.wait(lock, [&] { return !taskQueue.empty() || !running; });

        if (!running && taskQueue.empty()) break;
        std::shared_ptr<Task> task = taskQueue.front();
        taskQueue.pop();
        lock.unlock();

        std::cout << "Worker assigned task. Task Id : " << task->getTaskId() << std::endl;
        
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

        std::cout << "Result of task Id : " << task->getTaskId() << " " << task->getResult() << std::endl;
        // Push completed task into result queue
        {
            std::lock_guard<std::mutex> resLock(resultMutex); //consturctor call for lock_guard
            taskResultQueue.emplace(task);
        }

        resultCv.notify_one();
    }
}

void Scheduler::resultDispatcher(){
    std::cout << "Result dispatcher started." << std::endl;
    while(running){
        std::unique_lock<std::mutex> lock(resultMutex);
        // Wait until there is a task to send or the scheduler is stopping
        resultCv.wait(lock, [&]{return !taskResultQueue.empty() || !running; });
        if(!running && taskResultQueue.empty())
            break;
        std::shared_ptr<Task> task = taskResultQueue.front();
        taskResultQueue.pop();
        lock.unlock();

        std::cout  << "client Id: " << task->getClientId() << "Task Id: " << task->getTaskId() << " result dispatching : " << task->getResult() << std::endl; 
        char resultMsg[MAX_RESULT_MSG_SIZE];
        snprintf(resultMsg, MAX_RESULT_MSG_SIZE, 
                "Task %d Result %s\n",
                task->getTaskId(), task->getResult().c_str());
        
        mqd_t clientMq = mq_open(task->getMqName().c_str(), O_WRONLY);
        if(clientMq == (mqd_t)-1){
            perror("Dispatcher Open result mq failed");
            continue;
        }
        size_t bytes_send = mq_send(clientMq, resultMsg, strlen(resultMsg)+1, 0);
        if(bytes_send  == -1){
            perror("mq_send failed");
        }else {
                std::cout << "[Dispatcher] Sent result for Task " << task->getTaskId()
                        << " to " << task->getMqName() << std::endl;
            }
        mq_close(clientMq);

        }
}
void Scheduler::stop(){
    running = false;
    cv.notify_all();
    resultCv.notify_all();
    for (auto &t : workers)
        if (t.joinable()) t.join();
    if(dispatcherThread.joinable())    dispatcherThread.join();
}