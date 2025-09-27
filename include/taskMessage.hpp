#ifndef TASK_MESSAGE
#define TASK_MESSAGE

struct TaskMessage{
    int operation;  //operation to be performed
    int operandType;  //number or string operand
    int operandCount; //counter for number operands
    int operands[20];   //Array for number operands
    char strOperand[256];   //Array for string operand
};

#endif //TASK_MESSAGE