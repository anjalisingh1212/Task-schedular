#ifndef OPERATION_HPP
#define OPERATION_HPP

/**
 * @brief Enumeration defining supported task operations.
 * 
 * Each operation type is assigned a unique integer value.
 * These values are used by both client and server components
 * to identify which computation should be performed.
 */
enum Operation {
    ADD = 1,           // Perform addition of numeric operands
    SUBTRACT,          // Perform subtraction of numeric operands
    MULTIPLY,          // Perform multiplication of numeric operands
    SORT_NUMBERS,      // Sort a list of numeric operands
    REVERSE_STRING     // Reverse a string operand
};

#endif // OPERATION_HPP
