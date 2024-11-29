// FILE: precedence_stack.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Header file for stack for precedence syntax analysis of expressions


#ifndef H_STACK
#define H_STACK

#include "precedence_tree.h"
#include "return_values.h"

// Declaration of stack item type
typedef enum STACK_ITEM_TYPE{
    TERMINAL,        // id, int, float
    OPERATR,         // +, -, *, /, <, >, <=, >=, ==, !=, (, )
    NON_TERMINAL_E,  // E - with operator without relation operator
    NON_TERMINAL_R,  // T - relation operator
    SHIFT,           // <
    REDUCE,          // >
    L_B,             // (
    R_B,             // )
}STACK_ITEM_TYPE;

// Declaration of stack item
typedef struct T_STACK_ITEM {
    struct T_STACK_ITEM *prev;
    T_TREE_NODE_PTR node;
    STACK_ITEM_TYPE type;
    T_TOKEN *token;     
} T_STACK_ITEM, *T_STACK_ITEM_PTR;

// Declaration of stack
typedef struct T_STACK {
    unsigned int countItems;
    T_STACK_ITEM_PTR top;         
} T_STACK, *T_STACK_PTR;

// Function declarations for initializing stack
void stack_init(T_STACK_PTR stack);

// Function declarations for pushing push new stack item on top of stack
T_RET_VAL stack_push(T_STACK_PTR stack, T_TOKEN *token, STACK_ITEM_TYPE type);

// Function declarations for poping item from top of stack, if stack is empty then nothing happens
void stack_pop(T_STACK_PTR stack);

// Function declarations for get top item of stack
T_STACK_ITEM_PTR stack_top(T_STACK_PTR stack);

// Function declarations for get top terminal item of stack
T_STACK_ITEM_PTR stack_top_terminal(T_STACK_PTR stack);

// Function declarations for insert less before top terminal
T_RET_VAL stack_insert_less(T_STACK_PTR stack, T_STACK_ITEM_PTR terminal);

// Function declarations for delete all items in stack and free memory, will be used in case of errors
void stack_dispose(T_STACK_PTR stack);

// Function declarations for checking if stack is empty
bool is_empty(T_STACK_PTR stack);

#endif // H_STACK
