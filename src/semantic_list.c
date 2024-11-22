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
T_LIST_PTR list_init(){
    T_LIST_PTR list = (T_LIST_PTR)malloc(sizeof(T_LIST));
    list->first = NULL;
    list->last = NULL;
    list->active = NULL;
    list->size = 0;
    return list;
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

/**
 * @brief Function for detect if element is leteral or non-literal, for varibles check if they are defined, and set used for variables
 * @param list Pointer to the list
 * @return RET_VAL_OK if everything is ok, otherwise return RET_VAL_INTERNAL_ERR
 */
RetVal list_set_literal(T_LIST_PTR list, T_SYM_TABLE *table){

    T_LIST_ELEMENT_PTR element = list->first;

    for(int i = 0; i < list->size && element != NULL; i++){

        if(element->node->token->type == IDENTIFIER){
            if (symtable_find_symbol(table, element->node->token->lexeme) == NULL) {
                return RET_VAL_SEMANTIC_UNDEFINED_ERR;
            }
            Symbol *symbol = symtable_find_symbol(table, element->node->token->lexeme);
            // Set used of variable to true
            symbol->data.var.used = true;

            if (symbol->data.var.type == VAR_FLOAT_NULL) element->literalType = LITERAL_FLOAT_NLL;
            if (symbol->data.var.type == VAR_INT_NULL) element->literalType = LITERAL_INT_NLL;
            if (symbol->data.var.type == VAR_FLOAT) element->literalType = NLITERAL_FLOAT;
            if (symbol->data.var.type == VAR_INT) element->literalType = NLITERAL_INT; 
            
        } 
        if(element->node->token->type == INT) element->literalType = LITERAL_INT;
        if(element->node->token->type == FLOAT) element->literalType = LITERAL_FLOAT;
        
        element = element->next;
    }
    return RET_VAL_OK;
}

/**
 * @brief Function for set next element to active
 * @param list Pointer to the list
 */
void list_next(T_LIST_PTR list){
    if(list->active != NULL) list->active = list->active->next;
}


/**
 * @brief Function for set what element will be prev of active element in list after operation
 * @param list Pointer to the list
 * @return Element that will be previus of active element, NULL if active element is firt element or if previus previus element is NULL
 */
T_LIST_ELEMENT_PTR list_get_red(T_LIST_PTR list){
    if(list->active == NULL) return NULL;
    if(list->active->prev == NULL) return NULL;
    if(list->active->prev->prev == NULL) return NULL;
    if(list->active->prev->prev->prev != NULL) return list->active->prev->prev;
    else return NULL;
}

/**
 * @brief Function for chceck if number has same value as integer
 * @param num Number to check
 * @return True if number is rounded, otherwise false
 */
bool is_rounded(float num) {
    return num == (int)num;
}

/**
 * @brief Function for simulate operation
 * @param list Pointer to the list
 * @param op1 First operand
 * @param op2 Second operand
 * @param operator Operator
 * @param wich Wich operation will be simulated
 * @return 0 = RET_VAL_OK if everything is ok, otherwise return 99 = RET_VAL_INTERNAL_ERR or 7 = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR
 */

RetVal createOperation(T_LIST_PTR list, T_LIST_ELEMENT_PTR op1, T_LIST_ELEMENT_PTR  operator, T_LIST_ELEMENT_PTR op2, POSITION_LITERAL wich){
        T_LIST_ELEMENT_PTR prevValue = list_get_red(list);
        
        switch (wich) {

        case LLF:{
        
            // Set result type of to node
            operator->node->resultType = TYPE_FLOAT_RESULT;
            // Set literal type of operator
            operator->literalType = LITERAL_FLOAT;
            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }
        

            break;
        }
        case LLI:{ 

            // Set result type of to node
            operator->node->resultType = TYPE_INT_RESULT;
            // Set literal type of operator
            operator->literalType = LITERAL_INT;
            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }

            break;
        }
        case NNF:{ 

            // Set result type of to node
            operator->node->resultType = TYPE_FLOAT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_FLOAT;
            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }
            
            break;
        }
        case NNI:{ 

            // Set result type of to node
            operator->node->resultType = TYPE_INT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_INT;
            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }
            
            break;
        }

        case LNI:{ 

            // Set result type of to node
            operator->node->resultType = TYPE_INT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_INT;

            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }
            
            break;
        }

        case LNF:{ 

            // Set result type of to node
            operator->node->resultType = TYPE_FLOAT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_FLOAT;

            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }
            
            break;
        }

        case NLFI:{
            // Set result type of to node
            operator->node->resultType = TYPE_FLOAT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_FLOAT;

            // Set convert to float for code generation
            op2->node->convertToFloat = true;
            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }

            break;
        }

        case LNFI:{ 
            // Set result type of to node
            operator->node->resultType = TYPE_FLOAT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_FLOAT;

            if (is_rounded(op1->node->token->value.floatVal)) op1->node->convertToInt = true;
            else return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            
            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;


            

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }
            break;
        }
        
        case NLIF:{
            // Set result type of to node
            operator->node->resultType = TYPE_FLOAT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_FLOAT;

            // Set convert to float for code generation
            op2->node->convertToFloat = true;
            
            if (is_rounded(op2->node->token->value.floatVal)) op2->node->convertToInt = true;
            else return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;


            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }
        }
            
            break;
            
        case LNIF:{
            // Set result type of to node
            operator->node->resultType = TYPE_FLOAT_RESULT;
            // Set literal type of operator
            operator->literalType = NLITERAL_FLOAT;

            // Set convert to float for code generation
            op1->node->convertToFloat = true;
            
            //Destroy op1 and op2 and operator
            free(op1);
            free(op2);
            list->size -= 2;

            // Linked list
            if(prevValue != NULL){
                prevValue->next = list->active;
                list->active->prev = prevValue;
            }else{
                list->first = list->active;
                list->active->prev = NULL;
            }


            break;

        }

        default:
            return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            break;
    }

    return RET_VAL_OK;
    
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