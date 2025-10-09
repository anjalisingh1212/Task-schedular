#ifndef IPC_CONFIG_HPP
#define IPC_CONFIG_HPP

// POSIX message queue name
#define TASK_QUEUE "/task_queue"

// Max message
#define TASK_QUEUE_SIZE 10

// Max task message size
#define MAX_TASK_MSG_SIZE sizeof(TaskMessage)

// Max result message size
#define MAX_RESULT_MSG_SIZE 512

#endif