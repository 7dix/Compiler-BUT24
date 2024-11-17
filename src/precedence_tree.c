// FILE: precdence_tree.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Implementation of trees for precedence syntax analysis of expressions


#include "precedence_tree.h"

/**
 * @brief Function to initialize tree
 * @param tree Pointer to the tree
 */
void tree_init(T_TREE_NODE_PTR *tree){
    *tree = NULL;
    return;
}


/**
 * @brief Function to create a new node of tree
 * @param token Pointer on stored token
 * @return Initialized node, if nothing fails, then return NULL
 */

T_TREE_NODE_PTR createNode(T_TOKEN *token){
    // Create a new node of tree
    T_TREE_NODE_PTR node = (T_TREE_NODE_PTR)malloc(sizeof(T_TREE_NODE));
    // Check if malloc was successful
    if (node == NULL) return NULL;

    // Initialize node
    node->left = NULL;
    node->right = NULL;
    node->token = token;

    // Return node
    return node;

}

/**
 * @brief Function to create a new subtree
 * @param tree Pointer to the tree
 * @param operator Pointer to the operator node
 * @param left Pointer to the left node
 * @param right Pointer to the right node
 * @return Root of tree
 */

T_TREE_NODE_PTR createSubTree(T_TREE_NODE_PTR *tree, T_TREE_NODE_PTR operator, T_TREE_NODE_PTR right, T_TREE_NODE_PTR left){

    if(*tree == NULL){
        operator->left = left;
        operator->right = right;
        *tree = operator;
        return operator;
    }

    T_TREE_NODE_PTR oldRoot = *tree;
    operator->right = oldRoot;
    operator->left = left;
    
    return operator;
}

/**
 * @brief Function to delete all nodes in tree and free memory
 * @param tree Pointer to the tree
 */
void tree_dispose(T_TREE_NODE_PTR *tree){
    if (*tree == NULL) return;
    tree_dispose(&(*tree)->left);
    tree_dispose(&(*tree)->right);
    free(*tree);
    *tree = NULL;
    return;
}



//SMAZAT
void Postorder (T_TREE_NODE_PTR root){
    if (root != NULL){
    
    Postorder(root->left);
    Postorder(root->right);
    printf("%s ", root->token->lexeme);
    }
}