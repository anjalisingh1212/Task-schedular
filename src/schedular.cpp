#include <iostream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <atomic>
#include <csignal>
#include <cerrno>
#include "scheduler.hpp"
#include "task.hpp"
#include "taskMessage.hpp"
#include "ipcConfig.hpp"

// Atomic flag to control loop
std::atomic_bool keepRunning(true);

// Signal handler for Ctrl+C
void handle_sigint(int){
    keepRunning = false;
}

Scheduler::Scheduler(int size) : queue(size), front(-1), rear(-1), maxSize(size){}

bool Scheduler::isempty() const{
    if (front == -1)    return true;
    return false;
}

bool Scheduler::isfull() const{
    if((rear+1)%maxSize == front)   return true;
    return false;
}

bool Scheduler::enqueue(const Task &t){
    if(isfull()){
        std::cout << "Task queue is full!" << std::endl; 
        return false;
    }
    if(isempty()){
        front = rear = 0;
    }
    else{
        rear = (rear+1) % maxSize;
    }
    queue[rear] = t; 
    return true;
}

bool Scheduler::dequeue(Task &t){
    if(isempty()){
        std::cout << "Task queue is empty!" << std::endl; 
        return false;
    }
    t = queue[front];
    if(front == rear){
        front = rear = -1;
    }else{
        front = (front+1)%maxSize;
    }
    return true;
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
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;
    
    mqd_t mq = mq_open(TASK_QUEUE, O_CREAT | O_RDWR, 0644, &attr);
    if(mq == (mqd_t)-1){
        perror("mq_open failed");
        return -1;
    }
    Scheduler schedule_task = Scheduler(10);
    std::cout << "Schedular started." << std::endl;
    while(keepRunning){
        std::cout << "Waiting for tasks..." << std::endl;
        TaskMessage msg;

        size_t bytes_rec = mq_receive(mq, (char*)&msg, MAX_MSG_SIZE, nullptr);
        if(bytes_rec == -1){
            if(errno == EINTR){
                // Interrupted by signal, check keepRunning
                continue;
            }
            else{
                perror("mq_receive failed");
                break;
            }
        }
        std::cout << "msg struct created: " << std::endl;
            std::cout << "operandtype " << msg.operandType << std::endl;
            std::cout << "operation " << msg.operation << std::endl;
            for(int i = 0; i < msg.operandCount; i++)
                std::cout << "operands " << msg.operands[i] << std::endl;
            std::cout << "str " << msg.strOperand << std::endl;
        Task t = Task(msg, nextTaskId);
        nextTaskId++;
        schedule_task.enqueue(t);
        
}
    std::cout << "Schedular exiting..." << std::endl; 
    mq_close(mq);
    mq_unlink(TASK_QUEUE);
    return 0;
}