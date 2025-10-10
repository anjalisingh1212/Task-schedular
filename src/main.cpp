#include <iostream>
#include <mqueue.h>
#include <csignal>
#include <memory>
#include "scheduler.hpp"
#include "taskMessage.hpp"
#include "ipcConfig.hpp"


// Atomic flag to control loop
std::atomic_bool keepRunning(true);

// Signal handler for Ctrl+C
void handle_sigint(int){
    keepRunning = false;
}

int main(){

    //Set up signal handler without automatic restart
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; // Disables automatic restart of syscalls
    sigaction(SIGINT, &sa, nullptr);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = TASK_QUEUE_SIZE;
    attr.mq_msgsize = MAX_TASK_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    mqd_t mq = mq_open(TASK_QUEUE, O_CREAT | O_RDWR, 0644, &attr);
    if(mq == (mqd_t)-1){
        perror("mq_open failed");
        return -1;
    }

    Scheduler scheduler(3);
    std::cout << "Schedular started. Waiting for tasks..." << std::endl;

    while(keepRunning){
        TaskMessage msg;

        size_t bytes_rec = mq_receive(mq, (char*)&msg, MAX_TASK_MSG_SIZE, nullptr);

        if(bytes_rec >= 0){
            auto taskPtr = std::make_shared<Task>(msg);
            scheduler.enqueueTask(taskPtr);
        }else{
            if (errno == EINTR && !keepRunning) break;
        }
    }

    scheduler.stop();
    mq_close(mq);
    mq_unlink(TASK_QUEUE);
    std::cout << "Schedular stopped." <<std::endl; 

    return 0;
}