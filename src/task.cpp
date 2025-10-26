#include "task.hpp"
#include "taskMessage.hpp"
#include "operation.hpp"

/**
 * @brief Constructs a Task object from a TaskMessage structure.
 * 
 * This constructor initializes a Task using the details received
 * from an interprocess message (TaskMessage). It automatically 
 * determines whether the task involves numeric operands or a string,
 * and initializes fields accordingly.
 * 
 * @param msg TaskMessage structure containing task information.
 */
Task::Task(const struct TaskMessage &msg) {
    taskId = msg.taskId;
    clientId = msg.clientId;
    mqName = std::string(msg.resultMqName);  // Copy message queue name
    taskType = msg.operation;                // Operation type (e.g., ADD, CONCAT, etc.)
    // If operandType = 0 = numeric task (uses integer operands)
    if (msg.operandType == 0) {
        operands.assign(msg.operands, msg.operands + msg.operandCount);
    }
    // If operandType = 1 -> string-based task
    else if (msg.operandType == 1) {
        stringData = std::string(msg.strOperand);
        result = "";
    }
}

/**
 * @brief Sets the computed result of the task.
 * @param result The output string to store as the task result.
 */
void Task::setResult(const std::string result){
    this->result = result;
}

/**
 * @return Vector of integer operands for numeric tasks.
 */
std::vector<int> Task::getOperands() const {
    return operands;
}

/**
 * @return Input string data for string-based tasks.
 */
std::string Task::getStringData() const {
    return stringData;
}

/**
 * @return Unique task identifier.
 */
int Task::getTaskId() const {
    return taskId;
}

/**
 * @return Identifier of the client who submitted the task.
 */
int Task::getClientId() const {
    return clientId;
}

/**
 * @return Task type (numeric code representing operation type).
 */
int Task::getTaskType() const {
    return taskType;
}

/**
 * @return The result string of the task after execution.
 */
std::string Task::getResult() const {
    return result;
}

/**
 * @return The message queue name used for sending results back to the client.
 */
std::string Task::getMqName() const {
    return mqName;
}