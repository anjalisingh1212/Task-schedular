#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <atomic>
#include "task.hpp"

/*
 * Scheduler Class
 * ----------------
 * The Scheduler manages multiple worker threads that execute incoming tasks 
 * and a dispatcher thread that sends results back to clients via message queues.
 * 
 * Core Features:
 *  - Thread-safe task queuing using mutexes and condition variables
 *  - Parallel execution of tasks by worker threads
 *  - Separate result dispatcher thread for client communication
 *  - Graceful shutdown and resource cleanup
 */

class Scheduler{
private:
    // Queue holding tasks waiting to be executed
    std::queue <std::shared_ptr<Task>> taskQueue;

    // Queue holding completed tasks whose results need to be sent
    std::queue <std::shared_ptr<Task>> taskResultQueue;

    // Mutex to synchronize access to the task queue
    std::mutex queueMutex;

    // Mutex to synchronize access to the result queue
    std::mutex resultMutex;

    // Condition variable to notify worker threads when a new task arrives
    std::condition_variable cv;

    // Condition variable to notify dispatcher when a new result is ready
    std::condition_variable resultCv;

    // Pool of worker threads for parallel task execution
    std::vector<std::thread> workers;

    // Separate thread responsible for dispatching results to clients
    std::thread dispatcherThread;

    // Atomic flag used to indicate whether the scheduler is running
    std::atomic<bool> running;


public:

    /*
     * Constructor
     * Initializes the scheduler with the specified number of worker threads
     * and starts the dispatcher thread.
    */
    Scheduler(int numThreads);

    /*
     * Destructor
     * Ensures all threads are properly stopped and joined.
     */
    ~Scheduler();

    /*
     * enqueueTask()
     * Adds a new task to the queue for processing.
    */
    void enqueueTask(std::shared_ptr<Task>);

    /*
     * workerThread()
     * Function executed by each worker thread.
     * Waits for tasks, processes them, and pushes results into the result queue.
     */
    void workerThread();

    /*
     * resultDispatcher()
     * Function executed by the dispatcher thread.
     * Sends completed task results back to clients through message queues.
     */
    void resultDispatcher();

    /*
     * stop()
     * Gracefully shuts down all threads and cleans up resources.
     */
    void stop();
};

#endif //SCHEDULER_HPP