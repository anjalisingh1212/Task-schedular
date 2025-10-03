#ifndef IPC_CONFIG_HPP
#define IPC_CONFIG_HPP

// POSIX message queue name
#define TASK_QUEUE "/task_queue"

// Max message
#define TASK_QUEUE_SIZE 10

// Max message size
#define MAX_MSG_SIZE sizeof(TaskMessage)

#endif