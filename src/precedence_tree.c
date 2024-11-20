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
    bool convertToFloat = false;
    bool convertToInt = false;
    
    // Return node
    return node;

}

/**
 * @brief Function to create a subtree
 * @param operator Pointer to parent of subtree
 * @param right Pointer to right child of subtree
 * @param left Pointer to left child of subtree
 * @return Pointer to the root of subtree
 */
T_TREE_NODE_PTR createSubTree(T_TREE_NODE_PTR operator, T_TREE_NODE_PTR right, T_TREE_NODE_PTR left){
    operator->left = left;
    operator->right = right;
    return operator;
}

/**
 * @brief Function to delete all nodes in tree and free memory
 * @param tree Pointer to the tree
 */
void tree_dispose(T_TREE_NODE_PTR *tree){
    if (*tree == NULL) return;
    tree_dispose(&((*tree)->left));
    tree_dispose(&((*tree)->right));
    free(*tree);
    *tree = NULL;
    return;
}

/**
 * @brief Function for postorder tree traversal, for TESTING
 * @param root Pointer to the root of tree
 */
void postorderTest (T_TREE_NODE_PTR root){

    if (root == NULL)return;
    postorderTest(root->left);
    postorderTest(root->right);
    printf("%s ", root->token->lexeme);
    
}