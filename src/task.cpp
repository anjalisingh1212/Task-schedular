#include "task.hpp"

//Declarations
//Construtor
Task::Task(int Id, const std::string type) : taskId(Id), taskType(type){}

//Setters
void Task::setOperands(const std::vector<int> &operands){
    this->operands = operands;
}
void Task::setStringData(const std::string stringData){
    this->stringData = stringData;
}
void Task::setResult(const std::string result){
    this->result = result;
}

//Getters
std::vector<int> Task::getOperands() const {
    return operands;
}
int Task::getTaskId() const {
    return taskId;
}
std::string Task::getTaskType() const {
    return taskType;
}
std::string Task::getResult() const {
    return result;
}