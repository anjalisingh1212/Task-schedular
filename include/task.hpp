#ifndef TASK_HPP
#define TASK_HPP

#include <vector>
#include <string>

class Task{
private:
    int taskId;
    std::vector<int> operands;
    std::string stringData;
    std::string taskType;
    std::string result;

public:

    Task()=default;
    Task(int Id, const std::string type);
    //setters
    void setOperands(const std::vector<int> &);
    void setStringData(const std::string);
    void setResult(const std::string);

    //getters
    int getTaskId() const;
    std::string getTaskType() const;
    std::vector<int> getOperands() const;
    std::string getResult() const;

    //print information
    void print();

};

#endif //TASK_HPP