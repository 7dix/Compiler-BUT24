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
void tree_init(T_TREE_PTR tree){
    tree->countItems = 0;
    tree->first = NULL;
    tree->tree = NULL;
    return;
}

/**
 * @brief Function to insert first element to list of nodes of tree
 * @param tree Pointer to the tree
 * @param node Pointer to the node which will be inserted
 * @return 0 = RET_VAL_OK if everything is ok, 99 = RET_VAL_INTERNAL_ERR if malloc fails
 */
RetVal tree_insert_first(T_TREE_PTR tree, T_TREE_NODE_PTR node){

    // Create new element of tree list
    T_TREE_LIST_ELEMENT_PTR newElement = (T_TREE_LIST_ELEMENT_PTR)malloc(sizeof(T_TREE_LIST_ELEMENT));
    // Check if malloc was successful
    if (newElement == NULL) return RET_VAL_INTERNAL_ERR;

    // Initialize new element
    newElement->node = node;
    newElement->next = tree->first;

    // Set new element as root of tree
    tree->first = newElement;
    // Increment count of items in tree
    tree->countItems++;

    return RET_VAL_OK;
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
 * @brief Function to get tree from tree structure
 * @param tree Pointer to the tree
 * @return Pointer to the root of tree
 */
T_TREE_NODE_PTR getTree(T_TREE_PTR tree){
    if (tree == NULL) return NULL;
    return tree->tree;
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
void tree_dispose(T_TREE_PTR tree){
    if(tree == NULL) return;

    T_TREE_LIST_ELEMENT_PTR current = tree->first;
    while (current != NULL) {
        T_TREE_LIST_ELEMENT_PTR next = current->next; 
        free(current->node);                         
        free(current);                               
        current = next;                              
        tree->countItems--;                          
    }

    // Reset the tree structure
    tree->first = NULL;
    tree->tree = NULL;
    tree->countItems = 0;

    return;
}

/**
 * @brief Function for postorder tree traversal
 * @param root Pointer to the root of tree
 */
void postorder (T_TREE_NODE_PTR root){

    if (root == NULL)return;
    postorder(root->left);
    postorder(root->right);
    printf("%s ", root->token->lexeme);
    
}