#include <iostream>
#include <mqueue.h>
#include <csignal>
#include <cstring>
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

    int nextTaskId = 1;

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

        std::cout << "msg struct created: " << std::endl;
        std::cout << "operandtype " << msg.operandType << std::endl;
        std::cout << "operation " << msg.operation << std::endl;
        for(int i = 0; i < msg.operandCount; i++)
            std::cout << "operands " << msg.operands[i] << std::endl;
        std::cout << "str " << msg.strOperand << std::endl;

        if(bytes_rec >= 0){
            auto taskPtr = std::make_shared<Task>(msg, nextTaskId++);
            scheduler.enqueueTask(taskPtr);
        }else{
            if (errno == EINTR && !keepRunning) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }

    scheduler.stop();
    mq_close(mq);
    mq_unlink(TASK_QUEUE);
    std::cout << "Schedular stopped." <<std::endl; 

    return 0;
}