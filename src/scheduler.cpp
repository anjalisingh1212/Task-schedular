#include <iostream>
#include <algorithm>
#include <cstring>
#include <mqueue.h>
#include "scheduler.hpp"
#include "ipcConfig.hpp"

// Constructor: Initializes scheduler with given number of worker threads.
// Also starts a separate dispatcher thread for sending results to clients.
Scheduler::Scheduler(int numThreads) : running(true){
    // Start dispatcher thread to handle result messages
    dispatcherThread = std::thread(&Scheduler::resultDispatcher, this);
    // Create a pool of worker threads
    for(int i = 0; i < numThreads; i++){
        workers.emplace_back(&Scheduler::workerThread, this);
    }
}

// Destructor: ensures clean shutdown of all threads and resources.
Scheduler::~Scheduler(){
    stop();
}

// Adds a new task into the scheduler queue
void Scheduler::enqueueTask(std::shared_ptr<Task> task){
    std::cout << "Task is added in queue. Task Id: " << task->getTaskId() << std::endl;
    {
        //Lock the queue to ensure thread-safe access
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }
    // Notify one waiting worker that a new task is available
    cv.notify_one();
}

// Function executed by each worker thread
void Scheduler::workerThread(){
    std::cout << "Worker thread is started." << std::endl;
    // Continuously process tasks while scheduler is running
    while(running){
        std::unique_lock<std::mutex> lock(queueMutex);
        // Wait until there is a task to send or the scheduler is stopping
        cv.wait(lock, [&] { return !taskQueue.empty() || !running; });

        // If scheduler is stopping and queue is empty, exit thread
        if (!running && taskQueue.empty()) break;

        // Get next task from queue
        std::shared_ptr<Task> task = taskQueue.front();
        taskQueue.pop();
        lock.unlock();

        std::cout << "Worker assigned task. Task Id: " << task->getTaskId() << std::endl;
        
        // Simulate task execution
        std::string result; // Variable to hold computation result

        switch(task->getTaskType()){
            case 1: // Addition
            {
                int sum = 0;
                for(auto &op : task->getOperands())
                    sum += op;
                result = "Sum = " + std::to_string(sum);
                break;
            }
            case 2: // Subtraction
            {   
                const auto &op = task->getOperands();
                int diff = op[0];

                for(size_t i = 1; i < op.size(); i++)
                    diff -= op[i];
                result = "Difference = " + std::to_string(diff);
                break;
            }case 3: // Multiplication
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

        std::cout << "Result of task Id: " << task->getTaskId() << " , " << task->getResult() << std::endl;
        // Push completed task into result queue
        {
            std::lock_guard<std::mutex> resLock(resultMutex); //consturctor call for lock_guard
            taskResultQueue.emplace(task);
        }

        // Notify dispatcher that a result is ready
        resultCv.notify_one();
    }
}

// Function executed by dispatcher thread
// Sends completed task results to client message queues
void Scheduler::resultDispatcher(){
    std::cout << "Result dispatcher started." << std::endl;
    while(running){
        std::unique_lock<std::mutex> lock(resultMutex);
        // Wait until there is a task to send or the scheduler is stopping
        resultCv.wait(lock, [&]{return !taskResultQueue.empty() || !running; });

        // Exit if shutting down and queue is empty
        if(!running && taskResultQueue.empty())
            break;

        // Get the next completed task
        std::shared_ptr<Task> task = taskResultQueue.front();
        taskResultQueue.pop();
        lock.unlock();

        std::cout << "Result dispatching to client Id: " << task->getClientId() << " Task Id: " << task->getTaskId() << ". " << task->getResult() << std::endl; 

        // Prepare result message
        char resultMsg[MAX_RESULT_MSG_SIZE];
        snprintf(resultMsg, MAX_RESULT_MSG_SIZE, 
                "Task %d Result %s\n",
                task->getTaskId(), task->getResult().c_str());
        
        // Open client message queue for writing
        mqd_t clientMq = mq_open(task->getMqName().c_str(), O_WRONLY);
        if(clientMq == (mqd_t)-1){
            perror("Dispatcher Open result mq failed");
            continue;
        }

        // Send the result message to client
        size_t bytes_send = mq_send(clientMq, resultMsg, strlen(resultMsg)+1, 0);
        if(bytes_send  == -1){
            perror("mq_send failed");
        }else {
                std::cout << "[Dispatcher] Sent result for Task " << task->getTaskId()
                        << " to " << task->getMqName() << std::endl;
            }
        // Close the message queue descriptor
        mq_close(clientMq);

        }
}

// Stop all threads and cleans up resources
void Scheduler::stop(){
    running = false;
    cv.notify_all();
    resultCv.notify_all();
    // Join worker threads
    for (auto &t : workers)
        if (t.joinable()) t.join();
    // Join dispatcher thread
    if(dispatcherThread.joinable())    dispatcherThread.join();
}