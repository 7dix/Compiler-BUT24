// FILE: semantic_list.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Implementation of list of semantic functions

#include "semantic_list.h"

/**
 * @brief Function to initialize list
 * @param list Pointer to the list
 */
void list_init(T_LIST_PTR list){
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    return;
}


/**
 * @brief Function to insert last element to list
 * @param list Pointer to the list
 * @param node Pointer to the node
 * @return RET_VAL_OK if everything is ok, otherwise return RET_VAL_INTERNAL_ERR
 */
RetVal list_insert_last(T_LIST_PTR list, T_TREE_NODE_PTR node){
    

    // Create new element
    T_LIST_ELEMENT_PTR element = (T_LIST_ELEMENT_PTR)malloc(sizeof(T_LIST_ELEMENT));
    // Check if malloc was successful
    if(element == NULL) return RET_VAL_INTERNAL_ERR;

    // Initialize element
    element->node = node;
    element->next = NULL;
    element->prev = NULL;
    element->literalType = LITERAL_NON;

    // Set first and last element depending on if list is empty or not
    if(list->size == 0){
        list->first = element;
        list->last = element;
    
    // Insert element to the end of list;
    }else{
        // Save old last element
        T_LIST_ELEMENT_PTR odlLast = list->last;
        // Set new last element
        element->prev = odlLast;
        // Set new last element
        list->last = element;
        // Set next element of old last element
        odlLast->next = element;
        // Set next element of new element
        element->next = NULL;
    }
    // Incrementing size and return success
    list->size++;
    return RET_VAL_OK;
}
/**
 * @brief Function for do it postorder travel
 * @param tree Pointer on the tree
 * @param list Pointer to the list where we want to insert elements
 * @return RET_VAL_OK if everything is ok, otherwise return RET_VAL_INTERNAL_ERR
 */
RetVal postorder(T_LIST_PTR list, T_TREE_NODE_PTR *tree){
    if (*tree == NULL) return RET_VAL_OK;
    postorder(list, &(*tree)->left);
    postorder(list, &(*tree)->right);
    return list_insert_last(list, *tree);
    
}

/**
 * @brief Function to get postfix notation from tree to list
 * @param list Pointer to the list
 * @param node Pointer to the root of tree
 * @return RET_VAL_OK if everything is ok, otherwise return RET_VAL_INTERNAL_ERR
 */
RetVal list_get_postfix_notation(T_LIST_PTR list, T_TREE_NODE_PTR *tree){

    // Check if internal failure
    if(*tree == NULL) return RET_VAL_INTERNAL_ERR;
    
    if(postorder(list, tree)){
        // If something fails, and some elements are in list, then dispose list
        if(list->size > 0) list_dispose(list);
        return RET_VAL_INTERNAL_ERR;
    }
    else return RET_VAL_OK;
}

/**
 * @brief Function for get first element of list
 * @param list Pointer to the list
 * @return Pointer to the first element of list
 */
T_LIST_ELEMENT_PTR list_get_first(T_LIST_PTR list){
    return list->first;
}

RetVal list_set_literal(T_LIST_PTR list, T_SYM_TABLE *table){
    T_LIST_ELEMENT_PTR element = list->first;
    for(int i = 0; i < list->size && element != NULL; i++){
        if(element->node->token->type == IDENTIFIER){
            if (symtable_find_symbol(table, element->node->token->lexeme) == NULL) {
                return RET_VAL_SEMANTIC_UNDEFINED_ERR;
            }
            Symbol *symbol = symtable_find_symbol(table, element->node->token->lexeme);
            if (symbol->type == VAR_FLOAT_NULL) element->literalType = LITERAL_FLOAT_NLL;
            else if (symbol->type == VAR_INT_NULL) element->literalType = LITERAL_INT_NLL;
            else if (symbol->type == VAR_FLOAT) element->literalType = LITERAL_FLOAT;
            else if (symbol->type == VAR_INT) element->literalType = LITERAL_INT;
            
            
        } 
        else if(element->node->token->type == INT) element->literalType = LITERAL_INT;
        else if(element->node->token->type == FLOAT) element->literalType = LITERAL_FLOAT;
        
        element = element->next;
    }
        
}


/**
 * @brief Function for delete all list
 * @param list Pointer to the list
 */
void list_dispose(T_LIST_PTR list){
    // Check if list is empty
    if(list == NULL || list->size == 0) return;

    // Temporary variables
    T_LIST_ELEMENT_PTR element = list->first;
    T_LIST_ELEMENT_PTR nextElement = NULL;


    // Free all elements in list
    while(list->size > 0 && element != NULL){
        // Save next element
        nextElement = element->next;
        // Free current element
        free(element);
        // Move to next element
        element = nextElement;
        //Decrement size
        list->size--;        
    }
    // Set default values
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    return;
}