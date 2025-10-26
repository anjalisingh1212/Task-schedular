#include <iostream>
#include <mqueue.h>
#include <csignal>
#include <memory>
#include "scheduler.hpp"
#include "taskMessage.hpp"
#include "ipcConfig.hpp"


// Global atomic flag to control the main loop
std::atomic_bool keepRunning(true);

// Signal handler for shutdown (Ctrl+C)
void handle_sigint(int){
    keepRunning = false; // Set flag to false to stop the loop
}

int main(){

    //Set up signal handler without automatic restart of system calls
    struct sigaction sa;
    sa.sa_handler = handle_sigint;      // Assign our handler function
    sigemptyset(&sa.sa_mask);           // No signals blocked during handler
    sa.sa_flags = 0;                    // Disables automatic restart of syscalls
    sigaction(SIGINT, &sa, nullptr);    // Register handler for SIGINT

    // Configure message queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;                      // Blocking mode
    attr.mq_maxmsg = TASK_QUEUE_SIZE;       // Maximum number of messages
    attr.mq_msgsize = MAX_TASK_MSG_SIZE;    // Maximum size of each message
    attr.mq_curmsgs = 0;                    // Current number of messages = 0
    
    // Create or open the task message queue
    mqd_t mq = mq_open(TASK_QUEUE, O_CREAT | O_RDWR, 0644, &attr);
    if(mq == (mqd_t)-1){
        perror("mq_open failed");
        return -1;
    }

    // Create a Scheduler object with 3 worker threads
    Scheduler scheduler(3);
    std::cout << "Schedular started. Waiting for tasks..." << std::endl;

    // Main loop to continously recieve and process tasks
    while(keepRunning){
        TaskMessage msg;

        // Wait for a new task message from the queue
        size_t bytes_rec = mq_receive(mq, (char*)&msg, MAX_TASK_MSG_SIZE, nullptr);

        if(bytes_rec >= 0){
            // Wrap recieved message into a Task object and enqueue it for execution
            auto taskPtr = std::make_shared<Task>(msg);
            scheduler.enqueueTask(taskPtr);
        }else{
            // If interrupted by signal and program is stopping, break the loop
            if (errno == EINTR && !keepRunning) break;
        }
    }

    // Shutdown
    scheduler.stop();       // Stop worker threads safely
    mq_close(mq);           // Close message queue descriptor
    mq_unlink(TASK_QUEUE);  // Remove message queue from system
    std::cout << "Schedular stopped." <<std::endl; 

    return 0;
}