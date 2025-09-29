#include <iostream>
#include <vector>
#include "scheduler.hpp"
#include "task.hpp"

Scheduler::Scheduler(int size){
    maxSize = size;
    std::vector<Task> queue(maxSize);
    front = -1;
    rear = -1;
}

bool Scheduler::isempty() const{
    if (front == -1)    return true;
    return false;
}

bool Scheduler::isfull() const{
    if((rear+1)%maxSize == front)   return true;
    return false;
}

bool Scheduler::enqueue(const Task &t){
    if(isempty()){
        front = 0;
        rear = 0;
    }
    if(isfull()){
        std::cout << "Task queue is full!" << std::endl; 
        return false;
    }
    queue[rear++] = t; 
    return true;
}

bool Scheduler::dequeue(Task &t){
    if(isempty()){
        std::cout << "Task queue is empty!" << std::endl; 
        return false;
    }
    if(front == rear){
        front = -1;
        rear = -1;
        return true;
    }
    front = (front+1)%maxSize;
    return true;
}

int main(){

    return 0;
}