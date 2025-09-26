#include <iostream>
#include <vector>
#include "task.hpp"

int main(){
    std::cout << "=====================" << std::endl;
    std::cout << "Task Dirtibutor" << std::endl;
    std::cout << "=====================" << std::endl;

    int choice = 0;
    while(1){
        std::cout << "For selecting a task enter the serial number:" << std::endl;
        std::cout << "1. Addition" << std::endl;
        std::cout << "2. Subtraction" << std::endl;
        std::cout << "3. Multiplication" << std::endl;
        std::cout << "4. Sort Numbers" << std::endl;
        std::cout << "5. Reverse String" << std::endl;
        std::cout << "0. Exit" << std::endl;
        std::cout << "Enter: ";
        std::cin >> choice;
        if(choice == 0){
            std::cout << "Exiting..." << std::endl;
            break;
        }
        switch(choice){
            case 1: //Addition
            {
                int size;
                std::cout << "Enter number of operands to add: ";
                std::cin >> size;
                std::vector<int> ops(size);
                for(int i = 0; i < size; i++)
                {
                    std::cin >> ops[i];
                }
                break;
            }
            case 2: //Subtraction
            {
                int size;
                std::cout << "Enter number of operands to subtract: ";
                std::cin >> size;
                std::vector<int> ops(size);
                for(int i = 0; i < size; i++)
                {
                    std::cin >> ops[i];
                }
                break;
            }
            case 3: //Multiplication
            {
                int size;
                std::cout << "Enter number of operands to multiply: ";
                std::cin >> size;
                std::vector<int> ops(size);
                for(int i = 0; i < size; i++)
                {
                    std::cin >> ops[i];
                }
                break;
            }
            case 4: //Sort numbers
            {
                int size;
                int element;
                std::cout << "Enter number of elements: ";
                std::cin >> size;
                std::vector<int> arr(size);
                for(int i = 0; i < size; i++){
                    std::cin >> arr[i];
                }
                break;
            }
            case 5: //Reverse String
            {
                std::string str;
                std::cout << "Enter a string: ";
                std::getline(std::cin, str); 
                break;
            }
            default:
                std::cout << "No matching task!" << std::endl;

        }
    }

    return 0;
}