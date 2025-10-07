#ifndef TASK_HPP
#define TASK_HPP

#include <vector>
#include <string>

// Max result message size
#define MAX_RESULT_MSG_SIZE 512

class Task{
private:
    int taskId;
    int clientId;
    int taskType;
    std::vector<int> operands;
    std::string stringData;
    std::string result;
    std::string mqName;
public:

    Task()=default;
    Task(const struct TaskMessage &msg, int id);
    //setters
    void setOperands(const std::vector<int> &);
    void setStringData(const std::string);
    void setResult(const std::string);

    //getters
    int getTaskId() const;
    int getClientId() const;
    int getTaskType() const;
    std::vector<int> getOperands() const;
    std::string getStringData() const;
    std::string getResult() const;
    std::string getMqName() const;
};

#endif //TASK_HPP