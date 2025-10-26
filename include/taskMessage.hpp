#ifndef TASK_MESSAGE
#define TASK_MESSAGE

#define MAX_MQ_NAME_LEN 64   // Maximum length for the message queue name
#define STR_LEN 256          // Maximum length for string operands

/**
 * @brief Structure representing a task message exchanged between processes.
 * 
 * This structure is used to serialize and transmit task information
 * through POSIX message queues (mqueue). It carries all the data required
 * to reconstruct a Task object on the receiver side.
 */
struct TaskMessage {
    int taskId;                             // Unique identifier for the task
    int clientId;                           // Identifier for the client submitting the task
    char resultMqName[MAX_MQ_NAME_LEN];     // Name of the message queue to send results back
    int operation;                          // Type of operation (e.g., Add, Subtract, Sort, etc.)
    int operandType;                        // 0 → numeric operands, 1 → string operand
    int operandCount;                       // Number of operands (used when operandType = 0)
    int operands[20];                       // Array storing numeric operands
    char strOperand[STR_LEN];               // Buffer storing string operand (used when operandType = 1)
};

#endif // TASK_MESSAGE
