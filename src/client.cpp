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

std::atomic_bool keepRunning(true);

void handle_sigint(int){
    keepRunning = false;
}
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

TaskMessage createTask(int choice, int taskId, std::string MqName){
    TaskMessage msg = {};
    msg.taskId = taskId;
    msg.clientId = getpid();
    strncpy(msg.resultMqName, MqName.c_str(), MAX_MQ_NAME_LEN - 1);
    msg.resultMqName[MAX_MQ_NAME_LEN - 1] = '\0';
    msg.operation = choice;
    if(choice == ADD || choice == SUBTRACT || choice == MULTIPLY || choice == SORT_NUMBERS){

        int size;

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

        msg.operandType = 0;

        for(int i = 0; i < size; i++)   msg.operands[i] = ops[i];
        msg.operandCount = size;

    }else if(choice == REVERSE_STRING){
        std::string str;
        std::cout << "Enter a string: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        std::getline(std::cin, str); 
        size_t len = str.copy(msg.strOperand, STR_LEN-1);
        msg.strOperand[len] = '\0';
        msg.operandType = 1;
    }

    return msg;
}

void recieveResult(std::string MqName){

    std::cout << "Receiver thread is started" << std::endl;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_msgsize = MAX_RESULT_MSG_SIZE;
    attr.mq_maxmsg = TASK_QUEUE_SIZE;
    attr.mq_curmsgs = 0;
    char resMqName[MAX_MQ_NAME_LEN];
    strncpy(resMqName, MqName.c_str(), MAX_MQ_NAME_LEN - 1);
    resMqName[MAX_MQ_NAME_LEN - 1] = '\0';

    mqd_t resMq = mq_open(resMqName, O_CREAT | O_RDONLY, 0644, &attr);
    if(resMq == (mqd_t)-1){
        perror("[Client] Resceive Mq open failed");
        return;
    }

    char result[MAX_RESULT_MSG_SIZE] = "\0";

    while(keepRunning){
        memset(result, 0, sizeof(result));
        size_t bytesRec = mq_receive(resMq, result, MAX_RESULT_MSG_SIZE, 0);
        if (bytesRec < 0){
            if(errno == EINTR && !keepRunning) {   
                break;
            }
            else if(errno != EAGAIN){ 
            perror("[Client] mq_receive failed");
            break;
            }
        }
        if(bytesRec >= 0){
            if(strncmp(result, "EXIT", strlen("EXIT")) == 0)
                break;
            std::cout << "\nResult recived for " <<  result << std::endl;
        } 

    }

    mq_close(resMq);
    mq_unlink(resMqName);
    std::cout << "[Client] Receiver thread exiting...\n";

}

int main(){

    std::cout << "=====================" << std::endl;
    std::cout << "   Task Dirtibutor   " << std::endl;
    std::cout << "=====================" << std::endl;

    int taskId = 1;
    mqd_t taskMq = mq_open(TASK_QUEUE, O_WRONLY);
    if(taskMq == (mqd_t)-1){
        perror("mq_open failed");
        return -1;
    }

    std::string MqName = "/result_mq_" + std::to_string(getpid());
    
    std::thread recieverThread(&recieveResult, MqName);
    int choice = 0;
    while(1){
        showMenu();
        std::cin >> choice;
        if(choice == 0){
            keepRunning = false; // Signals receiver thread to exit
            char tempMqName[MAX_MQ_NAME_LEN];
            strncpy(tempMqName, MqName.c_str(), MAX_MQ_NAME_LEN);
            mqd_t tempMq = mq_open(tempMqName, O_WRONLY);
            mq_send(tempMq, "EXIT", strlen("EXIT"), 1);
            std::cout << "Exiting..." << std::endl;
            mq_close(tempMq);
            break;
        }
        if(choice > 5){
            std::cout << "No matching option..." << std::endl;
            continue;
        }

        TaskMessage msg = createTask(choice, taskId++, MqName);
        // std::cout << "msg struct created: " << std::endl;
        // std::cout << "operandtype " << msg.operandType << std::endl;
        // std::cout << "operation " << msg.operation << std::endl;
        // for(int i = 0; i < msg.operandCount; i++)
        //     std::cout << "operands " << msg.operands[i] << std::endl;
        // std::cout << "str " << msg.strOperand << std::endl;
        size_t bytes_send = mq_send(taskMq, (char*)&msg, MAX_TASK_MSG_SIZE, 0);
        if(bytes_send == -1){
            perror("[Client] Mq send failed");
            continue;
        }
    }

    mq_close(taskMq);
    if(recieverThread.joinable())
        recieverThread.join();
    std::cout << "Resource clean up done. Exit success." << std::endl;
    mq_unlink(MqName.c_str());
    return 0;
}