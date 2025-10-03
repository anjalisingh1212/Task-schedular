#include <iostream>
#include <vector>
#include <cstring>
#include <limits>
#include <fcntl.h>
#include <sys/types.h>
#include <mqueue.h>
#include "taskMessage.hpp"
#include "operation.hpp"
#include "ipcConfig.hpp"

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

TaskMessage createTask(int choice){
    TaskMessage msg = {};
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
        size_t len = str.copy(msg.strOperand, 255);
        msg.strOperand[len] = '\0';
        msg.operandType = 1;
    }
    return msg;
}
int main(){
    std::cout << "=====================" << std::endl;
    std::cout << "Task Dirtibutor" << std::endl;
    std::cout << "=====================" << std::endl;

    mqd_t mq = mq_open(TASK_QUEUE, O_WRONLY);
    if(mq == (mqd_t)-1){
        perror("mq_open failed");
        return -1;
    }
    int choice = 0;
    while(1){
        showMenu();
        std::cin >> choice;
        if(choice == 0){
            std::cout << "Exiting..." << std::endl;
            break;
        }
        if(choice > 5){
            std::cout << "No matching option..." << std::endl;
            continue;
        }

        TaskMessage msg = createTask(choice);
        std::cout << "msg struct created: " << std::endl;
        std::cout << "operandtype " << msg.operandType << std::endl;
        std::cout << "operation " << msg.operation << std::endl;
        for(int i = 0; i < msg.operandCount; i++)
            std::cout << "operands " << msg.operands[i] << std::endl;
        std::cout << "str " << msg.strOperand << std::endl;
        size_t bytes_send = mq_send(mq, (char*)&msg, MAX_MSG_SIZE, 0);
        if(bytes_send == -1){
            perror("mq_sned failed");
            continue;
        }
    }
    mq_close(mq);
    mq_unlink(TASK_QUEUE);
    return 0;
}