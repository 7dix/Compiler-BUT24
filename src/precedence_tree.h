// FILE: precedence_tree.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Header file for trees for precedence syntax analysis of expressions

#include "shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token_buffer.h"


#ifndef H_TREE
#define H_TREE


// Declaration of tree node
typedef struct T_TREE_NODE {
    struct T_TREE_NODE *left;
    struct T_TREE_NODE *right;
    T_TOKEN *token;
} T_TREE_NODE, *T_TREE_NODE_PTR;

// Declaration of tree list element
typedef struct T_TREE_LIST_ELEMENT {
    struct T_TREE_LIST_ELEMENT *next;  
    T_TREE_NODE_PTR node;
} T_TREE_LIST_ELEMENT, *T_TREE_LIST_ELEMENT_PTR;

// Declaration of tree
typedef struct T_TREE {
    T_TREE_LIST_ELEMENT_PTR first;     
    unsigned int countItems;
    T_TREE_NODE_PTR tree;
} T_TREE, *T_TREE_PTR;




// Function declaration for init tree
void tree_init(T_TREE_PTR tree);

// Function declaration for create node of tree
T_TREE_NODE_PTR createNode(T_TOKEN *token);

// Function declaration for create subtree
T_TREE_NODE_PTR createSubTree(T_TREE_NODE_PTR operator, T_TREE_NODE_PTR left, T_TREE_NODE_PTR right);

// Function declaration for delete all nodes in tree and free memory
void tree_dispose(T_TREE_PTR tree);

// Function declaration for get tree
T_TREE_NODE_PTR getTree(T_TREE_PTR tree);

// Function declaration for insert first element to list of nodes of tree
RetVal tree_insert_first(T_TREE_PTR tree, T_TREE_NODE_PTR node);


// Function declaration for postorder tree traversal
void postorder (T_TREE_NODE_PTR root);

#endif // H_TREE