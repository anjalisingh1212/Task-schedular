#include "task.hpp"
#include "taskMessage.hpp"
#include "operation.hpp"

//Declarations
//Construtor
Task::Task(const struct TaskMessage &msg, int id) {
    taskId = id;
    clientId = msg.clientId;
    mqName = std::string(msg.resultMqName);
    taskType = msg.operation;
    //numeric operation
    if(msg.operandType == 0){
        operands.assign(msg.operands, msg.operands + msg.operandCount);
    }else if(msg.operandType == 1){
        stringData = std::string(msg.strOperand);
        result = "";
    }
}

//Setter
void Task::setResult(const std::string result){
    this->result = result;
}

//Getters
std::vector<int> Task::getOperands() const {
    return operands;
}
std::string Task::getStringData() const {
    return stringData;
}
int Task::getTaskId() const {
    return taskId;
}
int Task::getClientId() const {
    return clientId;
}
int Task::getTaskType() const {
    return taskType;
}
std::string Task::getResult() const {
    return result;
}
std::string Task::getMqName() const {
    return mqName;
}