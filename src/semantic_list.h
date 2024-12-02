// FILE: semantic_list.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Header file for list of semantic functions


#include "return_values.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token_buffer.h"
#include "precedence_tree.h"
#include "symtable.h"


#ifndef SEMANTIC_LIST_H
#define SEMANTIC_LIST_H

// Declaration of result of operation, literal or non-literal
typedef enum LITERAL_TYPE{
    LITERAL_NOT_SET,
    LITERAL_INT,
    LITERAL_FLOAT,
    LITERAL_NULL,
    NLITERAL_INT,
    NLITERAL_FLOAT,
    NLITERAL_INT_NULL,
    NLITERAL_FLOAT_NULL,
    NLITERAL_STRING,
    NLITERAL_STRING_NULL,
    LITERAL_STRING,
    NLITERAL_NULL,
    OPERATOR_OF_EXPR,
} LITERAL_TYPE;

// Declaration of type of operator for set witch types can be used
typedef enum OPERATOR_TYPE_OF_RULE{
    ARITMETIC,    // +, -, *
    DIVISION,     // /
    RELATIONAL,   // <, >, <=, >=
    EQUALITY,     // ==, !=
}OPERATOR_TYPE_OF_RULE; 

// Declaration of struct for list element
typedef struct T_LIST_ELEMENT {
    struct T_LIST_ELEMENT *next;
    struct T_LIST_ELEMENT *prev;    
    T_TREE_NODE_PTR node;
    LITERAL_TYPE literal_type;
    float value;
} T_LIST_ELEMENT, *T_LIST_ELEMENT_PTR;

// Declaration of struct for list
typedef struct T_LIST {
    T_LIST_ELEMENT_PTR first;
    T_LIST_ELEMENT_PTR last;
    T_LIST_ELEMENT_PTR active;
    int size;
} T_LIST, *T_LIST_PTR;


// Function declaration for init list
T_LIST_PTR list_init();

// Function declaration for get type of operator, ofr get what type of rytape can be used
OPERATOR_TYPE_OF_RULE get_operator_type(T_LIST_ELEMENT_PTR operator);

// Function declaration for insert element to end of list
T_RET_VAL list_insert_last(T_LIST_PTR list, T_TREE_NODE_PTR node);

// Function declaration for postorder tree traversal
T_RET_VAL postorder( T_TREE_NODE_PTR *tree, T_LIST_PTR list);

// Function declaration for set types of operands
T_RET_VAL set_types(T_LIST_PTR list, T_SYM_TABLE *table);

// Function declaration for set first element as active
void list_first(T_LIST_PTR list);

// Function declaration for set next element as active
void list_next(T_LIST_PTR list);

// Function declaration for delete two items from list after active element
void list_delete_two_after(T_LIST_PTR list);

// Function declaration for compare float and int
bool is_float_int(float num);

// Function declaration for delete list and free memory
void list_dispose(T_LIST_PTR list);

#endif // SEMANTIC_LIST_H