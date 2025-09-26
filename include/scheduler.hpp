#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <vector>
#include "task.hpp"

class Scheduler{
private:
    std::vector<Task> queue;
    int front;
    int rear;
    int maxSize;

public:
    Scheduler(int);
    bool isempty() const;
    bool isfull() const;
    bool enqueue(const Task &t);
    bool dequeue(Task &t); 
};

#endif //SCHEDULER_HPP