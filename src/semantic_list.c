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
 * @brief Function for set witch retyping depending on the operator can be used
 * @param operator List element with operator
 * @return Number of "group" of operators
 */
OPERATOR_TYPE_OF_RULE get_operator_type(T_LIST_ELEMENT_PTR operator){
    // +, -, *
    if (operator->node->token->type == PLUS || operator->node->token->type == MINUS || operator->node->token->type == MULTIPLY) return ARITMETIC;
    // /
    if (operator->node->token->type == DIVIDE) return DIVISION;
    // <, >, <=, >=
    if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL) return EQUALITY;
    return RELATIONAL;
    
}

/**
 * @brief Function to insert element to the list
 * @param list Pointer to the list
 * @param data Pointer to the data
 * @return 0=RET_VAL_OK if the element was inserted, else return 99=RET_VAL_INTERNAL_ERR if malloc fails
 */
RET_VAL list_insert_last(T_LIST_PTR list, T_TREE_NODE_PTR node){
    // Check if list is empty
    if(list == NULL) return RET_VAL_INTERNAL_ERR;

    // Create new element
    T_LIST_ELEMENT_PTR element = (T_LIST_ELEMENT_PTR)malloc(sizeof(T_LIST_ELEMENT));
    // Check if malloc was successful
    if(element == NULL) return RET_VAL_INTERNAL_ERR;

    // Initialize element
    element->prev = NULL;
    element->next = NULL;
    element->node = node;
    element->value = 0.1;
    element->literal_type = LITERAL_NOT_SET;

    // Check if list is empty, and insert element
    if(list->size == 0){
        list->first = element;
        list->last = element;
        list->active = element;
    }else{
        list->last->next = element;
        element->prev = list->last;
        list->last = element;
    }

    // Incrementing size
    list->size++;
    return RET_VAL_OK;
}

/**
 * @brief Function for postorder tree traversal
 * @param tree Pointer on the tree
 * @param list Pointer to the list, where be stored nodes
 * @return 0=RET_VAL_OK if the tree was traversed, else return 99=RET_VAL_INTERNAL_ERR if malloc fails
 */
RET_VAL postorder(T_TREE_NODE_PTR *tree, T_LIST_PTR list){
    
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
RET_VAL set_types(T_LIST_PTR list, T_SYM_TABLE *table){

    
    list_first(list);
    while(list->active != NULL){

        // IDENTIFIER
        if(list->active->node->token->type == IDENTIFIER){
            // Find symbol in the symbol table
            T_SYMBOL *symbol = symtable_find_symbol(table, list->active->node->token->lexeme);

            // Check if symbol is in the symbol table, else return error of undefined variable
            if(symbol == NULL) return RET_VAL_SEMANTIC_UNDEFINED_ERR;
            // If identifer is in the symbol table, set used flag
            else symbol->data.var.used = true;
            
            // For constant variables
            if(symbol->data.var.is_const){
                switch (symbol->data.var.type){ // const :i32
                case VAR_INT:{
                    list->active->literal_type = NLITERAL_INT;
                    break;
                }
                case VAR_FLOAT:{ // const :f64
                    list->active->literal_type = LITERAL_FLOAT;
                    if (symbol->data.var.const_expr) list->active->value = (float)symbol->data.var.float_value;
                    break;
                }
                default:{
                    break;
                }
            }
            //For var varialbes
            }else{
                switch (symbol->data.var.type){
                    case VAR_INT:{ // var :i32
                        list->active->literal_type = NLITERAL_INT;
                        break;
                    }
                    case VAR_FLOAT:{ // var :f64
                        list->active->literal_type = NLITERAL_FLOAT;
                        break;
                    }
                    default:{
                        break;
                    }
                }
            }

            // var :?i32 | const :?i32
            if(symbol->data.var.type == VAR_INT_NULL) list->active->literal_type = NLITERAL_INT_NULL;
            
            // var :?f64 | const :?f64
            if(symbol->data.var.type == VAR_FLOAT_NULL) list->active->literal_type = NLITERAL_FLOAT_NULL;

            // var :?[]u8 | const :[]u8          
            if(symbol->data.var.type == VAR_STRING_NULL) list->active->literal_type = NLITERAL_STRING_NULL;
            
            // var :[]u8 | const :[]u8
            if (symbol->data.var.type == VAR_STRING) list->active->literal_type = NLITERAL_STRING;
            
            // Error of not set type
            if(symbol->data.var.type == VAR_VOID) return RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR;

            list_next(list);
            continue;

        }
        
        // NULL
        if(list->active->node->token->type == NULL_TOKEN){
            list->active->literal_type = LITERAL_NULL;
            list_next(list);
            continue;
        }
        // LITERAL INT
        if(list->active->node->token->type == INT){
            list->active->literal_type = LITERAL_INT;
            list_next(list);
            continue;
        }

        // LITERAL FLOAT
        if(list->active->node->token->type == FLOAT){
            list->active->literal_type = LITERAL_FLOAT;
            list->active->value = (float)list->active->node->token->value.float_val;
            list_next(list);
            continue;
        }

        // LITERAL STRING
        if(list->active->node->token->type == STRING){
            list->active->literal_type = LITERAL_STRING;
            list_next(list);
            continue;
        }

        // OPERATOR
        if(list->active->node->token->type == PLUS || list->active->node->token->type == MINUS || list->active->node->token->type == MULTIPLY || list->active->node->token->type == DIVIDE || list->active->node->token->type == LESS_THAN || list->active->node->token->type == GREATER_THAN || list->active->node->token->type == LESS_THAN_EQUAL || list->active->node->token->type == GREATER_THAN_EQUAL || list->active->node->token->type == EQUAL || list->active->node->token->type == NOT_EQUAL){
            list->active->literal_type = OPERATOR_OF_EXPR;
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
    T_LIST_ELEMENT_PTR delete_one = list->active->prev;
    T_LIST_ELEMENT_PTR delete_two = delete_one->prev;

    // If delete_two is first element, set active like first item
    if(list->first == delete_two) list->first = list->active;
    

    // Linked list depending on the position of the active element
    if(delete_two->prev == NULL){ 
         list->active->prev = NULL;
    }
    else{  
        list->active->prev = delete_two->prev;
        delete_two->prev->next = list->active;
    }

    // Destroy links
    delete_one->prev = NULL;
    delete_one->next = NULL;
    delete_two->prev = NULL;
    delete_two->next = NULL;

    // Delete elements
    free(delete_one);
    free(delete_two);

    // Decrement size
    list->size = list->size - 2;
    
    if(list->size == 1) list->last = list->active;
    
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
    T_LIST_ELEMENT_PTR next_element = NULL;


    // Free all elements in list
    while(list->size != 0){
        // Save next element
        next_element = element->next;
        // Free current element
        free(element);
        // Move to next element
        element = next_element;
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