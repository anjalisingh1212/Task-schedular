#ifndef TASK_HPP
#define TASK_HPP

#include <vector>
#include <string>

// Forward declaration to avoid including taskMessage.hpp here,
// reducing unnecessary dependencies
struct TaskMessage;

/**
 * @class Task
 * @brief Represents a unit of work submitted by a client.
 *
 * Each Task encapsulates the metadata and data needed for execution,
 * such as operands, string input, and result data. It also holds
 * identifying information like task ID, client ID, and message queue name.
 */

class Task{
private:
     int taskId;                     // Unique identifier for the task
    int clientId;                   // ID of the client who submitted the task
    int taskType;                   // Type/category of the task (e.g., computation, string processing)
    std::vector<int> operands;      // Numeric data for computation tasks
    std::string stringData;         // Input string for text-related tasks
    std::string result;             // Result after task execution
    std::string mqName;             // Message queue name to send results back to the client
public:
/**
     * @brief Default constructor
     */
    Task() = default;

    /**
     * @brief Construct a Task object from a TaskMessage struct
     * @param msg The message structure containing task details
     */
    Task(const struct TaskMessage &msg);

    // ------------------- Setters -------------------

    /**
     * @brief Set operands for the task
     * @param ops Vector of integer operands
     */
    void setOperands(const std::vector<int> &ops);

    /**
     * @brief Set string data for the task
     * @param data Input string
     */
    void setStringData(const std::string data);

    /**
     * @brief Set the result of the task after execution
     * @param res Result string
     */
    void setResult(const std::string res);

    // ------------------- Getters -------------------

    /**
     * @return Task ID
     */
    int getTaskId() const;

    /**
     * @return Client ID
     */
    int getClientId() const;

    /**
     * @return Task type
     */
    int getTaskType() const;

    /**
     * @return Operands for computation
     */
    std::vector<int> getOperands() const;

    /**
     * @return Input string data
     */
    std::string getStringData() const;

    /**
     * @return Result string
     */
    std::string getResult() const;

    /**
     * @return Message queue name for result dispatch
     */
    std::string getMqName() const;
};

#endif //TASK_HPP