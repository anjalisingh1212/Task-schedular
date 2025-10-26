#include <iostream>
#include <vector>
#include <cstring>
#include <limits>
#include <fcntl.h>
#include <sys/types.h>
#include <mqueue.h>
#include <unistd.h>
#include <atomic>
#include <thread>
#include <signal.h>
#include "taskMessage.hpp"
#include "operation.hpp"
#include "ipcConfig.hpp"

// Global flag used to control program termination (shared across threads)
std::atomic_bool keepRunning(true);

// Signal handler for Ctrl+C to stop client gracefully
void handle_sigint(int){
    keepRunning = false;
}

// Display the available operations to the user
void showMenu(){ 
    std::cout << "For selecting a task enter the serial number:" << std::endl;
    std::cout << "1. Addition" << std::endl;
    std::cout << "2. Subtraction" << std::endl;
    std::cout << "3. Multiplication" << std::endl;
    std::cout << "4. Sort Numbers" << std::endl;
    std::cout << "5. Reverse String" << std::endl;
    std::cout << "0. Exit" << std::endl;
    std::cout << "Enter: ";
}

/*
 * createTask()
 * -------------
 * Builds a TaskMessage structure based on user's selected operation.
 * The message is later sent to the scheduler via the task message queue.
 */
TaskMessage createTask(int choice, int taskId, std::string MqName){
    TaskMessage msg = {};
    msg.taskId = taskId;
    msg.clientId = getpid(); // Use process ID as unique client identifier

    // Copy client’s result queue name into message (for scheduler to respond)
    strncpy(msg.resultMqName, MqName.c_str(), MAX_MQ_NAME_LEN - 1);
    msg.resultMqName[MAX_MQ_NAME_LEN - 1] = '\0';
    msg.operation = choice;

    // For numeric operations: ADD, SUBTRACT, MULTIPLY, SORT
    if(choice == ADD || choice == SUBTRACT || choice == MULTIPLY || choice == SORT_NUMBERS){
        int size;

        // Validate operand count
        while(1){
        std::cout << "Enter number of operands(max 20 operands): ";
        std::cin >> size;

            if(size > 20){
                std::cout << "Entered number is more than allowed 20!" << std::endl;
            }else break;
        }

        std::vector<int> ops(size);
        std::cout << "Enter the " << size << " operands: ";
        for(int i = 0; i < size; i++)
        {
            std::cin >> ops[i];
        }

        msg.operandType = 0; // Numeric type

        for(int i = 0; i < size; i++)   msg.operands[i] = ops[i];
        msg.operandCount = size;

    }else if(choice == REVERSE_STRING){ // For string operation
        std::string str;
        std::cout << "Enter a string: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        std::getline(std::cin, str);
        
        // Copy string to message buffer safely
        size_t len = str.copy(msg.strOperand, STR_LEN-1);
        msg.strOperand[len] = '\0';
        msg.operandType = 1; // String type
    }

    return msg;
}

/*
 * recieveResult()
 * ----------------
 * This function runs in a separate thread.
 * It continuously listens for results from the scheduler via a message queue.
 */
void recieveResult(std::string MqName){

    std::cout << "Receiver thread is started" << std::endl;

    // Define message queue attributes
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_msgsize = MAX_RESULT_MSG_SIZE;
    attr.mq_maxmsg = TASK_QUEUE_SIZE;
    attr.mq_curmsgs = 0;

    // Create and open client’s result message queue
    char resMqName[MAX_MQ_NAME_LEN];
    strncpy(resMqName, MqName.c_str(), MAX_MQ_NAME_LEN - 1);
    resMqName[MAX_MQ_NAME_LEN - 1] = '\0';

    mqd_t resMq = mq_open(resMqName, O_CREAT | O_RDONLY, 0644, &attr);
    if(resMq == (mqd_t)-1){
        perror("[Client] Resceive Mq open failed");
        return;
    }

    char result[MAX_RESULT_MSG_SIZE] = "\0";

    // Continuously listen for results until interrupted
    while(keepRunning){
        memset(result, 0, sizeof(result));
        size_t bytesRec = mq_receive(resMq, result, MAX_RESULT_MSG_SIZE, 0);
        if (bytesRec < 0){
            if(errno == EINTR && !keepRunning) {    // Interrupted by signal
                break;
            }
            else if(errno != EAGAIN){  // Other errors
            perror("[Client] mq_receive failed");
            break;
            }
        }

        // Handle received message
        if(bytesRec >= 0){
            // Exit signal from main thread
            if(strncmp(result, "EXIT", strlen("EXIT")) == 0)
                break;
            std::cout << "\nResult recived for " <<  result << std::endl;
        } 

    }

    // Cleanup message queue before exiting
    mq_close(resMq);
    mq_unlink(resMqName);
    std::cout << "[Client] Receiver thread exiting...\n";

}

/*
 * main()
 * -------
 * Entry point for the client program.
 * - Connects to the scheduler’s task queue
 * - Creates a private result queue for receiving results
 * - Starts receiver thread
 * - Allows user to continuously submit tasks until exit
 */
int main(){

    std::cout << "=====================" << std::endl;
    std::cout << "   Task Dirtibutor   " << std::endl;
    std::cout << "=====================" << std::endl;

    int taskId = 1;

    // Connect to the scheduler's task message queue
    mqd_t taskMq = mq_open(TASK_QUEUE, O_WRONLY);
    if(taskMq == (mqd_t)-1){
        perror("mq_open failed");
        return -1;
    }

     // Create a unique message queue name for this client (based on PID)
    std::string MqName = "/result_mq_" + std::to_string(getpid());
    
    // Start a separate thread to listen for results
    std::thread recieverThread(&recieveResult, MqName);

    int choice = 0;
    while(1){
        showMenu();
        std::cin >> choice;

        // Exit option
        if(choice == 0){
            keepRunning = false; // Signals receiver thread to exit

            // Send EXIT message to result queue to unblock mq_receive
            char tempMqName[MAX_MQ_NAME_LEN];
            strncpy(tempMqName, MqName.c_str(), MAX_MQ_NAME_LEN);
            mqd_t tempMq = mq_open(tempMqName, O_WRONLY);
            mq_send(tempMq, "EXIT", strlen("EXIT"), 1);
            std::cout << "Exiting..." << std::endl;
            mq_close(tempMq);
            break;
        }

        // Invalid option
        if(choice > 5){
            std::cout << "No matching option..." << std::endl;
            continue;
        }

        // Create task message and send it to scheduler
        TaskMessage msg = createTask(choice, taskId++, MqName);
        size_t bytes_send = mq_send(taskMq, (char*)&msg, MAX_TASK_MSG_SIZE, 0);
        if(bytes_send == -1){
            perror("[Client] Mq send failed");
            continue;
        }
    }

    // Clean up and close resources
    mq_close(taskMq);
    if(recieverThread.joinable())
        recieverThread.join();
    std::cout << "Resource clean up done. Exit success." << std::endl;
    mq_unlink(MqName.c_str());
    return 0;
}