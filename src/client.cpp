#include <iostream>
#include <vector>
#include <cstring>
#include "taskMessage.hpp"
#include "operation.hpp"

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
    TaskMessage msg;
    msg.operation = choice;
    if(choice == ADD || choice == SUBTRACT || choice == MULTIPLY || choice == SORT_NUMBERS){
        int size;
        std::cout << "Enter number of operands: ";
        std::cin >> size;
        std::vector<int> ops(size);
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
        
    }

    return 0;
}