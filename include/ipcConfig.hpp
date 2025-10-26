#ifndef IPC_CONFIG_HPP
#define IPC_CONFIG_HPP

/**
 * @brief Global IPC configuration constants.
 * 
 * This header defines configuration parameters for
 * the POSIX message queue–based interprocess communication
 * between the client and the scheduler/server.
 */

// Name of the main task queue used for client → scheduler communication
#define TASK_QUEUE "/task_queue"

// Maximum number of messages allowed in the task queue
#define TASK_QUEUE_SIZE 10

// Maximum size (in bytes) of each task message
#define MAX_TASK_MSG_SIZE sizeof(TaskMessage)

// Maximum size (in bytes) for result messages sent back to clients
#define MAX_RESULT_MSG_SIZE 512

#endif // IPC_CONFIG_HPP
