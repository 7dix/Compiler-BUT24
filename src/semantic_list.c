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
 * @return Pointer to the list
 */
T_LIST_PTR list_init(){
    T_LIST_PTR list = (T_LIST_PTR)malloc(sizeof(T_LIST));
    list->first = NULL;
    list->last = NULL;
    list->active = NULL;
    list->size = 0;
    return list;
}

/**
 * @brief Function to insert element to the list
 * @param list Pointer to the list
 * @param data Pointer to the data
 * @return 0=RET_VAL_OK if the element was inserted, else return 99=RET_VAL_INTERNAL_ERR if malloc fails
 */
RetVal list_insert_last(T_LIST_PTR list, T_TREE_NODE_PTR node){
    // Check if list is empty
    if(list == NULL) return RET_VAL_INTERNAL_ERR;

    // Create new element
    T_LIST_ELEMENT_PTR element = (T_LIST_ELEMENT_PTR)malloc(sizeof(T_LIST_ELEMENT));
    // Check if malloc was successful
    if(element == NULL) return RET_VAL_INTERNAL_ERR;

   
    element->next = NULL;
    element->node = node;
    element->literalType = LITERAL_NOT_SET;

    // Check if list is empty
    if(list->size == 0){
        list->first = element;
        list->last = element;
        list->active = element;
    }else{
        list->last->next = element;
        list->last = element;
    }

    list->size++;
    return RET_VAL_OK;
}

/**
 * @brief Function for postorder tree traversal
 * @param tree Pointer on the tree
 * @param list Pointer to the list, where be stored nodes
 * @return 0=RET_VAL_OK if the tree was traversed, else return 99=RET_VAL_INTERNAL_ERR if malloc fails
 */
RetVal postorder(T_TREE_NODE_PTR *tree, T_LIST_PTR list){
    
    if(*tree == NULL) return RET_VAL_OK;

    // Postorder traversal
    if((*tree)->right != NULL) postorder(&((*tree)->left), list);
    
    if((*tree)->right != NULL) postorder(&((*tree)->right), list);

    // Insert node to list
    return list_insert_last(list, *tree);
}
/**
 * @brief Function to set first element as active
 * @param list Pointer to the list
 */
void list_first(T_LIST_PTR list){
    if(list == NULL || list->size == 0) return;
    list->active = list->first;
}

/**
 * @brief Function to set next element as active
 * @param list Pointer to the list
 */
void list_next(T_LIST_PTR list){
    if(list == NULL || list->size == 0 || list->active == NULL) return;
    if(list->active == list->last){
        list->active = NULL;
        return;
    }
    list->active = list->active->next;
}

/**
 * @brief Function to set types of operands
 * @param list Pointer to the list
 * @param table Pointer to the symbol table
 * @return 0=RET_VAL_OK if the types was set, else return 99=RET_VAL_INTERNAL_ERR if malloc fails
 */
RetVal set_types(T_LIST_PTR list, T_SYM_TABLE *table){

    list_first(list);
    while(list->active != NULL){

        // IDENTIFIER
        if(list->active->node->token->type == IDENTIFIER){
            // Find symbol in the symbol table
            Symbol *symbol = symtable_find_symbol(table, list->active->node->token->lexeme);

            // Check if symbol is in the symbol table, else return error of undefined variable
            if(symbol == NULL) return RET_VAL_SEMANTIC_UNDEFINED_ERR;
            // If identifer is in the symbol table, set used flag
            else symbol->data.var.used = true;

            // Search fot the type of identifier
            switch (symbol->data.var.type){
                case VAR_INT:{
                    list->active->literalType = NLITERAL_INT;
                    break;
                }
                case VAR_INT_NULL:{
                    list->active->literalType = NLITERAL_INT_NULL;
                    break;
                }
                case VAR_FLOAT:{
                    list->active->literalType = NLITERAL_FLOAT;
                    break;
                }
                case VAR_FLOAT_NULL:{
                    list->active->literalType = NLITERAL_FLOAT_NULL;
                    break;
                }
                case VAR_VOID:{
                    // TODO: NEEDS CHECK IF IT IS OK
                    return RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR;
                    
                }
                case VAR_STRING_NULL:{
                    list->active->literalType = NELITERAL_STRING_NULL;
                    break;
                }
                case VAR_STRING:{
                    list->active->literalType = NELITERAL_STRING;
                    break;
                }
                
                default:{
                    // If type is not of the above, return error of semantic type compatibility 
                    return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
                }
            }
            list_next(list);
            continue;

        }
        
        // NULL
        if(list->active->node->token->type == NULL_TOKEN){
            list->active->literalType = LITERAL_NULL;
            list_next(list);
            continue;
        }
        // LITERAL INT
        if(list->active->node->token->type == INT){
            list->active->literalType = LITERAL_INT;
            list_next(list);
            continue;
        }

        // LITERAL FLOAT
        if(list->active->node->token->type == FLOAT){
            list->active->literalType = LITERAL_FLOAT;
            list_next(list);
            continue;
        }

        // OPERATOR
        if(list->active->node->token->type == PLUS || list->active->node->token->type == MINUS || list->active->node->token->type == MULTIPLY || list->active->node->token->type == DIVIDE || list->active->node->token->type == LESS_THAN || list->active->node->token->type == GREATER_THAN || list->active->node->token->type == LESS_THAN_EQUAL || list->active->node->token->type == GREATER_THAN_EQUAL || list->active->node->token->type == EQUAL || list->active->node->token->type == NOT_EQUAL){
            list->active->literalType = LITERAL_NOT_SET;
            list_next(list);
            continue;
        }

        // Error of necompatibility of types
        return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;            
    }

    return RET_VAL_OK;
}
/**
 * @brief Function for delete two items after active element
 * @param list Pointer to the list
 */
void list_delete_two_after(T_LIST_PTR list){
    
    // Elements for delete
    T_LIST_ELEMENT_PTR deleteOne = list->active->prev;
    T_LIST_ELEMENT_PTR deleteTwo = deleteOne->prev;

    // Linked list depending on the position of the active element
    if(deleteTwo->prev == NULL) list->active->prev = NULL;
    else list->active->prev = deleteTwo->prev;

    // Delete elements
    free(deleteOne);
    free(deleteTwo);

    // Decrement size
    list->size = list->size - 2;

    return;
}

/**
 * @brief Function for compare float and int
 * @param float Float to comapare
 * @return true if float is equal to int, else false 
 * */ 
bool is_float_int(float floatNumber){
    return floatNumber == (int)floatNumber;
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
    list->active = NULL;
    list->size = 0;
    free(list);
    return;
}