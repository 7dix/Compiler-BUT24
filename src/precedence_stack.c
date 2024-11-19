// FILE: precedence_stack.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Stack implementation for precedence syntax analysis of expressions

#include "precedence_stack.h"

/**
 * @brief Function for initializing stack
 * @param stack Pointer on stack for initialization
*/
void stack_init(T_STACK_PTR stack) {
    stack->countItems = 0;
    stack->top = NULL;
}


/**
 * @brief Function for checking if stack is empty
 * @param stack Pointer on stack for checking
 * @return True if stack is empty, false otherwise
*/
bool is_empty(T_STACK_PTR stack){
    return stack->countItems == 0 || stack->top == NULL;
}

/**
 * @brief Function for pushing push new stack item on top of stack
 * @param stack Pontier on stack where item will be pushed
 * @param token Token for pushing item
 * @param type Type of item on stack
 * @return 99 = RET_VAL_INTERNAL_ERR if everything is ok, 0 = RET_VAL_OK if everything is ok
 */
RetVal stack_push(T_STACK_PTR stack, T_TOKEN *token, STACK_ITEM_TYPE type){

    // Create new item of stack
    T_STACK_ITEM_PTR itemPush = (T_STACK_ITEM_PTR)malloc(sizeof(T_STACK_ITEM));

    // Check if malloc was successful
    if(itemPush == NULL) return RET_VAL_INTERNAL_ERR;

    // Check if type is terminal or operator, this is important for tree node
    if (type == SHIFT || type == REDUCE || type == L_B || type == R_B || type == NON_TERMINAL_E || type == NON_TERMINAL_R){
        itemPush->node = NULL;
        itemPush->token = token;
        itemPush->type = type;
        itemPush->prev = NULL;

    }else{
    
        // Create new node of tree
        T_TREE_NODE_PTR node = createNode(token);

        // Check if node was created
        if(node == NULL) return RET_VAL_INTERNAL_ERR;
            
        // Initialize item of stack
        itemPush->node = node;
        itemPush->type = type;
        itemPush->token = token;
        itemPush->prev = NULL;

    }
    

    // Set token like top of stack, depending on if stack is empty or not
    if (is_empty(stack)){
        stack->top = itemPush;
    } else {
        itemPush->prev = stack->top;
        stack->top = itemPush;
    }

    // Increment count of items in stack
    stack->countItems++;

    return RET_VAL_OK;
}

/**
 * @brief Function for poping item from top of stack, if stack is empty then nothing happens
 * @param stack Pointer on stack wher will be token poped
*/
void stack_pop(T_STACK_PTR stack){

    // Check if stack is not empty
    if (is_empty(stack)) return;

    // Save popped item
    T_STACK_ITEM_PTR tokenPop = stack->top;

    // Set new top of stack, based if stack has anothers items or not
    if (stack->countItems == 1) stack->top = NULL;
    else stack->top = tokenPop->prev;
    
    // Delete popped item from memory
    tokenPop->prev = NULL;
    free(tokenPop);

    // Decrement count of items in stack
    stack->countItems--;
     
}

/**
 * @brief Function for get top item of stack
 * @param stack Pointer on stack 
 * @return Pointer on item from top of stack, if stack is empty then return NULL
*/
T_STACK_ITEM_PTR stack_top(T_STACK_PTR stack){
    if (is_empty(stack)) return NULL;
    else return stack->top;
}

/**
 * @brief Function for get top terminal item of stack
 * @param stack Pointer on stack 
 * @return Pointer on terminal item from top of stack, if stack has not terminal item then return NULL
*/
T_STACK_ITEM_PTR stack_top_terminal(T_STACK_PTR stack) {
    // Search for terminal item in stack
    T_STACK_ITEM_PTR searchTerminal = stack_top(stack);
    
    // Check if searchTerminal is not NULL
    if (searchTerminal == NULL) return NULL;
    
    while (searchTerminal != NULL) {
        if (searchTerminal->type == TERMINAL || searchTerminal->type == L_B || searchTerminal->type == R_B) {
            return searchTerminal;
        } else {
            searchTerminal = searchTerminal->prev;
        }
    }
    
    return NULL;
}

/**
 * @brief Function for insert less before top terminal
 * @param stack Pointer on stack where will be inserted less
 * @param terminal Pointer on terminal item where will be inserted less
 * @return 99 = RET_VAL_INTERNAL_ERR if everything is ok, 0 = RET_VAL_OK if everything is ok
 */
RetVal stack_insert_less(T_STACK_PTR stack, T_STACK_ITEM_PTR terminal){


    // Create new item of stack
    T_STACK_ITEM_PTR itemPush = (T_STACK_ITEM_PTR)malloc(sizeof(T_STACK_ITEM));

    // Check if malloc was successful
    if(itemPush == NULL) return RET_VAL_INTERNAL_ERR;

    // Initialize item of stack
    itemPush->node = NULL;
    itemPush->type = SHIFT;
    itemPush->token = NULL;
    itemPush->prev = NULL;

    // If terminal is on top of stack, then will be inserted less on top of stack
    if(terminal == stack_top(stack) && !is_empty(stack)){
        itemPush->prev = terminal;
        stack->top = itemPush;
        stack->countItems++;
        return RET_VAL_OK;
    // If terminal is on top of stack and stack is empty, then will be inserted less on top of stack
    }else if(terminal == NULL && is_empty(stack)){
        stack->top = itemPush;
        stack->countItems++;
        return RET_VAL_OK;
    
    }else if(terminal == NULL && !is_empty(stack)){
        T_STACK_ITEM_PTR begin = stack_top(stack);
        while (begin->prev != NULL) {
            begin = begin->prev;
        }

        begin->prev = itemPush;
        stack->countItems++;
        return RET_VAL_OK;
        
    }else{
        // Pointer on item before terminal, position where will be inserted less
        T_STACK_ITEM_PTR beforeTerminal = stack_top(stack);
        // Search for position before terminal
        while (beforeTerminal->prev != terminal) {
            beforeTerminal = beforeTerminal->prev;
            
        }

        itemPush->prev = terminal;
        beforeTerminal->prev = itemPush;
        stack->countItems++;

        return RET_VAL_OK;
    }
}
 


/**
 * @brief Function for delete all items in stack and free memory, will be used in case of errors
 * @param stack Pointer on stack where all items will be deleted
*/
void stack_dispose(T_STACK_PTR stack) {
    while (!is_empty(stack)) {
        tree_dispose(&(stack->top->node));
        stack_pop(stack);
    }
    
    stack->countItems = 0;
    stack->top = NULL;

    return;  
}