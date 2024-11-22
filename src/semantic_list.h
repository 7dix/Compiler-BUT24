// FILE: semantic_list.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Header file for list of semantic functions


#include "shared.h"
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
    LITERAL_NON,
    LITERAL_INT,
    LITERAL_FLOAT,
    NLITERAL_INT,
    NLITERAL_FLOAT,
    LITERAL_INT_NLL,
    LITERAL_FLOAT_NLL,
} LITERAL_TYPE;

// Declaration of position literal, and change type of prototyping
typedef enum POSITION_LITERAL{
    // Lf and lf
    LLF,
    // Li and li
    LLI,

    // NF and nf
    NNF,
    //Ni and ni
    NNI,

    //li and ni
    LNI,
    //li and nf
    NLI,
    //lf and nf
    LNF,
    //nf and lf
    NLF,

    //lf and ni
    LNFI,
    //nf and li
    NLFI,
    
    //li and lf
    NLIF,
    //li and nf
    LNIF,
    
}POSITION_LITERAL;
// Declaration of struct for list element
typedef struct T_LIST_ELEMENT {
    struct T_LIST_ELEMENT *next;
    struct T_LIST_ELEMENT *prev;    
    T_TREE_NODE_PTR node;
    LITERAL_TYPE literalType;
} T_LIST_ELEMENT, *T_LIST_ELEMENT_PTR;

// Declaration of struct for list
typedef struct T_LIST {
    T_LIST_ELEMENT_PTR first;
    T_LIST_ELEMENT_PTR last;
    T_LIST_ELEMENT_PTR active;
    int size;
} T_LIST, *T_LIST_PTR;


// Function declaration for init list
void list_init(T_LIST_PTR list);

// Function declaration for get postfix notation from tree to list
RetVal list_get_postfix_notation(T_LIST_PTR list, T_TREE_NODE_PTR *tree);

// Function declaration for do it postorder travel
RetVal postorder(T_LIST_PTR list, T_TREE_NODE_PTR *tree);

// Function declaration insert last element to list
RetVal list_insert_last(T_LIST_PTR list, T_TREE_NODE_PTR node);

// Function declaration for simulate operation
RetVal createOperation(T_LIST_PTR list, T_LIST_ELEMENT_PTR op1, T_LIST_ELEMENT_PTR  operator, T_LIST_ELEMENT_PTR op2, POSITION_LITERAL wich);

// Function declaration for dispose list
void list_dispose(T_LIST_PTR list);

// Function declaration for get first element of list
T_LIST_ELEMENT_PTR list_get_first(T_LIST_PTR list);

// Function declaration for insert element to list like first
RetVal list_insert_first(T_LIST_PTR list, T_TREE_NODE_PTR node);

// Function declaration for set literal type
RetVal list_set_literal(T_LIST_PTR list, T_SYM_TABLE *table);

// Function declaration for set next element as active
void list_next(T_LIST_PTR list);

// Function declaration for set wich element will be reduce by operation
T_LIST_ELEMENT_PTR list_get_red(T_LIST_PTR list);

// Function declaration for compare float and int
bool is_rounded(float num);

#endif // SEMANTIC_LIST_H