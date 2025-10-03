#ifndef TASK_HPP
#define TASK_HPP

#include <vector>
#include <string>

class Task{
private:
    int taskId;
    int taskType;
    std::vector<int> operands;
    std::string stringData;
    std::string result;

public:

    Task()=default;
    Task(const struct TaskMessage &msg, int id);
    //setters
    void setOperands(const std::vector<int> &);
    void setStringData(const std::string);
    void setResult(const std::string);

    //getters
    int getTaskId() const;
    int getTaskType() const;
    std::vector<int> getOperands() const;
    std::string getResult() const;

    //print information
    void print();

};

#endif //TASK_HPP