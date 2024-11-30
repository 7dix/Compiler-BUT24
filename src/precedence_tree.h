// FILE: precedence_tree.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Header file for trees for precedence syntax analysis of expressions


#ifndef H_TREE
#define H_TREE


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "token_buffer.h"


// Declaration of result type for semantic analysis
typedef enum RESULT_TYPE{
    TYPE_NOTSET_RESULT,
    TYPE_INT_RESULT,
    TYPE_FLOAT_RESULT,
    TYPE_BOOL_RESULT,
    TYPE_NULL_RESULT,
    TYPE_INT_NULL_RESULT,
    TYPE_FLOAT_NULL_RESULT,
    TYPE_STRING_RESULT,
    TYPE_STRING_NULL_RESULT,
    TYPE_STRING_LITERAL_RESULT,
} RESULT_TYPE;

// Declaration of tree node
typedef struct T_TREE_NODE {
    struct T_TREE_NODE *left;
    struct T_TREE_NODE *right;
    T_TOKEN *token;
    bool convertToFloat;
    bool convertToInt;
    RESULT_TYPE resultType;

} T_TREE_NODE, *T_TREE_NODE_PTR;



// Function declaration for init tree
void tree_init(T_TREE_NODE_PTR *tree);

// Function declaration for create node of tree
T_TREE_NODE_PTR createNode(T_TOKEN *token);

// Function declaration for create subtree
T_TREE_NODE_PTR createSubTree(T_TREE_NODE_PTR operator, T_TREE_NODE_PTR left, T_TREE_NODE_PTR right);

// Function declaration for delete all nodes in tree and free memory
void tree_dispose(T_TREE_NODE_PTR *tree);

// Function declaration for postorder tree traversal for TESTING
void postorderTest (T_TREE_NODE_PTR root);

#endif // H_TREE