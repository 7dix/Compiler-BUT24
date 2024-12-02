// FILE: semantic.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//  <Martin Zůbek> (253206)
//
// YEAR: 2024
// NOTES: Helper functions for semantic analysis

#include "semantic.h"


/**
 * @brief checks if the function call is valid
 * 
 * @param table Pointer to the symbol table
 * @param fn_call Pointer to the function call
 * @return int as defined in `RET_VAL` `return_values.h`
 * @retval 0=RET_VAL_OK if the function call is valid
 * @retval 1-9 if the function call is invalid
 */
int check_function_call(T_SYM_TABLE *table, T_FN_CALL *fn_call) {
    T_SYMBOL *fn = symtable_find_symbol(table, fn_call->name);
    if (fn == NULL || fn->type != SYM_FUNC) {
        return RET_VAL_SEMANTIC_UNDEFINED_ERR;
    }

    if (fn->data.func.argc != fn_call->argc) {
        return RET_VAL_SEMANTIC_FUNCTION_ERR;
    }

    // copy return type
    fn_call->ret_type = fn->data.func.return_type;

    //check all arguments in array argv
    for (int i = 0; i < fn_call->argc; i++) {

        switch (fn_call->argv[i]->type) {
            case IDENTIFIER:
            {
                // check if the identifier is in the symbol table
                T_SYMBOL *symbol = symtable_find_symbol(table, fn_call->argv[i]->lexeme);
                if (symbol == NULL) {
                    return RET_VAL_SEMANTIC_UNDEFINED_ERR;
                }
                // check if the identifier is a variable
                if (symbol->type != SYM_VAR) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR;
                }

                if (fn->data.func.argv[i].type == STRING_VAR_STRING &&
                    symbol->data.var.type == VAR_STRING) {

                    symbol->data.var.used = true;
                    break;
                }

                // check if the variable is of the correct type
                if (symbol->data.var.type != fn->data.func.argv[i].type &&
                    fn->data.func.argv[i].type != VAR_ANY) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR;
                }
                symbol->data.var.used = true;
                break;
            }
            case INT:
                if (fn->data.func.argv[i].type != VAR_INT &&
                    fn->data.func.argv[i].type != VAR_INT_NULL &&
                    fn->data.func.argv[i].type != VAR_ANY) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR;
                }
                break;
            
            case FLOAT:
                if (fn->data.func.argv[i].type != VAR_FLOAT &&
                    fn->data.func.argv[i].type != VAR_FLOAT_NULL &&
                    fn->data.func.argv[i].type != VAR_ANY) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR;
                }
                break;

            case STRING:
                if (fn->data.func.argv[i].type != STRING_VAR_STRING &&
                    fn->data.func.argv[i].type != VAR_ANY) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR;
                }
                break;
            case NULL_TOKEN:
                if (fn->data.func.argv[i].type != VAR_INT_NULL &&
                    fn->data.func.argv[i].type != VAR_FLOAT_NULL &&
                    fn->data.func.argv[i].type != VAR_STRING_NULL &&
                    fn->data.func.argv[i].type != VAR_ANY) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR;
                }
                break;

            default:
                // TOOD: correct return value?
                return RET_VAL_SEMANTIC_FUNCTION_ERR;
                break;
        }
    }

    return RET_VAL_OK;
}

/**
 * @brief Function for adding argument to function call
 * 
 * @param fn_call Pointer to the function call
 * @param arg Pointer to the argument
 * @return int as defined in `RET_VAL` `return_values.h`
 * @retval 0=RET_VAL_OK if the argument was added successfully
 * @retval 1=RET_VAL_INTERNAL_ERR if the memory allocation failed
 */
int add_arg_to_fn_call(T_FN_CALL *fn_call, T_TOKEN *arg) {
    if (fn_call->argc == 0) {
        fn_call->argv = (T_TOKEN **) malloc(sizeof(T_TOKEN *));
        if (fn_call->argv == NULL) {
            return RET_VAL_INTERNAL_ERR;
        }
    } else {
        T_TOKEN **new_argv = (T_TOKEN **) realloc(fn_call->argv, (fn_call->argc + 1) * sizeof(T_TOKEN *));
        if (new_argv == NULL) {
            return RET_VAL_INTERNAL_ERR;
        }
        fn_call->argv = new_argv;
    }
    fn_call->argv[fn_call->argc++] = arg;
    return RET_VAL_OK;
}

/**
 * @brief Function for freeing function call arguments
 * 
 * @param fn_call
 */
void free_fn_call_args(T_FN_CALL *fn_call) {
    if (fn_call->argv != NULL) {
        free(fn_call->argv);
    }
}

/**
 * @brief Function for checking type compatibility
 * 
 * @param existing Pointer to the existing variable type (left side)
 * @param new Pointer to the new variable type (right side)
 * @return int as defined in `RET_VAL` `return_values.h`
 * @retval 0=RET_VAL_OK if the types are compatible
 * @retval 1-9 if incompatible
 */
int compare_var_types(VAR_TYPE *existing, VAR_TYPE *new) {
    if (*existing == *new) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_INT_NULL && (*new == VAR_INT || *new == VAR_NULL)) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_FLOAT_NULL && (*new == VAR_FLOAT || *new == VAR_NULL)) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_STRING_NULL && (*new == VAR_STRING || *new == VAR_NULL)) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_STRING && *new == STRING_VAR_STRING) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_STRING_NULL && *new == STRING_VAR_STRING) {
        return RET_VAL_OK;
    }

    if (*new == VAR_BOOL) {
        return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
    }

    if (*new == VAR_ANY) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_NONE) {

        if (*new == STRING_LITERAL) {
            return RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR;
        }

        *existing = *new;
        return RET_VAL_OK;
    }

    return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
}

/**
 * @brief Function for semantic analysis of expression
 * @param table Pointer to the symbol table
 * @param tree Pointer to the root of the tree
 * @return 0=RET_VAL_OK if the expression is valid, otherwise return one of semnatic errors
 */
RET_VAL check_expression(T_SYM_TABLE *table, T_TREE_NODE_PTR *tree) {

    // Create and initialize list for postfix notation
    T_LIST_PTR list_postfix = list_init();

    // Check if list was created
    if(list_postfix == NULL) return RET_VAL_INTERNAL_ERR;
    // Postorder traversal, get list of nodes in postfix notation

    RET_VAL err_expr = postorder(tree, list_postfix);

    // Check if postorder traversal and creating list was successful
    if(err_expr){
        if(list_postfix->size > 0) list_dispose(list_postfix);
        return err_expr;
    }

    // Set types of operands
    err_expr = set_types(list_postfix, table);

    // Check compatibility of types
    if(err_expr){
        list_dispose(list_postfix);
        return err_expr;
    }
    // Set first element as active
    list_first(list_postfix);
   
   // Set result type of expression with one operand
    if(list_postfix->size == 1){
        switch (list_postfix->active->literal_type){
            case LITERAL_INT:{ 
                (*tree)->result_type = TYPE_INT_RESULT;
                break;
            }
            case LITERAL_FLOAT:{ 
                (*tree)->result_type = TYPE_FLOAT_RESULT;
                break;
            }
            case LITERAL_NULL:{ 
                (*tree)->result_type = TYPE_NULL_RESULT;
                break;
            }
            case NLITERAL_INT:{ 
                (*tree)->result_type = TYPE_INT_RESULT;
                break;
            }
            case NLITERAL_FLOAT:{ 
                (*tree)->result_type = TYPE_FLOAT_RESULT;
                break;
            }
            case NLITERAL_INT_NULL:{ 
                (*tree)->result_type = TYPE_INT_NULL_RESULT;
                break;
            }
            case NLITERAL_FLOAT_NULL:{ 
                (*tree)->result_type = TYPE_FLOAT_NULL_RESULT;
                break;
            }
            case NLITERAL_STRING:{ 
                (*tree)->result_type = TYPE_STRING_RESULT;
                break;
            }
            case NLITERAL_STRING_NULL:{ 
                (*tree)->result_type = TYPE_STRING_NULL_RESULT;
                break;
            }
            case LITERAL_STRING:{ 
                (*tree)->result_type = TYPE_STRING_LITERAL_RESULT;
                break;
            }
            default:{
                list_dispose(list_postfix);
                return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            }
        }
        // Dispose list
        list_dispose(list_postfix);
        // Return OK
        return RET_VAL_OK;
    }

    while (list_postfix->size != 1 && list_postfix->active != NULL) {

        // Reduce the expression only if the active element is operator
        if(list_postfix->active->literal_type != OPERATOR_OF_EXPR) {
            list_next(list_postfix);
            continue;
        }

        // Set the last three elements in the list
        T_LIST_ELEMENT_PTR operator = list_postfix->active;
        T_LIST_ELEMENT_PTR operand_second = operator->prev;
        T_LIST_ELEMENT_PTR operand_first = operand_second->prev;

        // Get the type of operator
        OPERATOR_TYPE_OF_RULE operatorType = get_operator_type(operator);

        // Result is NONLITERAL INT or BOOL
        if((operand_first->literal_type == LITERAL_INT && operand_second->literal_type == LITERAL_INT) || (operand_first->literal_type == NLITERAL_INT && operand_second->literal_type == LITERAL_INT) || (operand_first->literal_type == LITERAL_INT && operand_second->literal_type == NLITERAL_INT) || (operand_first->literal_type == NLITERAL_INT && operand_second->literal_type == NLITERAL_INT)){
            // Set type of subexpression
            switch (operatorType){
                case ARITMETIC:{ 
                    operator->node->result_type = TYPE_INT_RESULT;
                    break;
                }
                case RELATIONAL:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                case EQUALITY:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                case DIVISION:{ 
                    operator->node->result_type = TYPE_INT_RESULT;
                    break;
                }
            }
            // Set type of literal
            operator->literal_type = NLITERAL_INT;
            // Delete two elements after active element, and move to next element
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Result is NONLITERAL FLOAT or BOOL
        if((operand_first->literal_type == LITERAL_FLOAT && operand_second->literal_type == LITERAL_FLOAT) || (operand_first->literal_type == NLITERAL_FLOAT && operand_second->literal_type == LITERAL_FLOAT) || (operand_first->literal_type == LITERAL_FLOAT && operand_second->literal_type == NLITERAL_FLOAT) || (operand_first->literal_type == NLITERAL_FLOAT && operand_second->literal_type == NLITERAL_FLOAT)){
            // Set type of subexpression
            switch (operatorType){
                case ARITMETIC:{ 
                    operator->node->result_type = TYPE_FLOAT_RESULT;
                    break;
                }
                case RELATIONAL:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                case EQUALITY:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                case DIVISION:{ 
                    operator->node->result_type = TYPE_FLOAT_RESULT;
                    break;
                }
            }
            // Set type of literal
            operator->literal_type = NLITERAL_FLOAT;
            // Delete two elements after active element, and move to next element
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Result is NLITERAL FLOAT after rytype of INT, does not working for division
        if((operatorType != DIVISION) && ((operand_first->literal_type == LITERAL_INT && operand_second->literal_type == NLITERAL_FLOAT) || (operand_first->literal_type == NLITERAL_FLOAT && operand_second->literal_type == LITERAL_INT) || (operand_first->literal_type == LITERAL_INT && operand_second->literal_type == LITERAL_FLOAT) || (operand_first->literal_type == LITERAL_FLOAT && operand_second->literal_type == LITERAL_INT))){
            // Set type of subexpression
            switch (operatorType){
                case ARITMETIC:{ 
                    operator->node->result_type = TYPE_FLOAT_RESULT;
                    break;
                }
                case RELATIONAL:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                case EQUALITY:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                default:{
                    break;
                }
               
            }
            // Set type of literal
            operator->literal_type = NLITERAL_FLOAT;
            // Retype of INT to FLOAT
            if(operand_first->literal_type == LITERAL_INT) operand_first->node->convert_to_float = true;
            else operand_second->node->convert_to_float = true;
            // Delete two elements after active element, and move to next element
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Result is NLITERAL INT after rytype of FLOAT
        if((operand_first->literal_type == LITERAL_FLOAT && operand_second->literal_type == NLITERAL_INT) || (operand_first->literal_type == NLITERAL_INT && operand_second->literal_type == LITERAL_FLOAT)){
            // Set type of subexpression
            switch (operatorType){
                case ARITMETIC:{ 
                    operator->node->result_type = TYPE_INT_RESULT;
                    break;
                }
                case RELATIONAL:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                case EQUALITY:{ 
                    operator->node->result_type = TYPE_BOOL_RESULT;
                    break;
                }
                case DIVISION:{ 
                    operator->node->result_type = TYPE_INT_RESULT;
                    break;
                }
            }
            // Set type of literal
            operator->literal_type = NLITERAL_INT;
            // Retype of FLOAT to INT, only if the value of float has evrything after the decimal point 0
            if(operand_first->literal_type == LITERAL_FLOAT && is_float_int(operand_first->value)) operand_first->node->convert_to_int = true;
            else if(operand_second->literal_type == LITERAL_FLOAT && is_float_int(operand_second->value)) operand_second->node->convert_to_int = true;
            else{
                list_dispose(list_postfix);
                return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            }
            // Delete two elements after active element, and move to next element
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }
        
        // Compare two NULLS
        if(operatorType == EQUALITY && (operand_first->literal_type == LITERAL_NULL || operand_second->literal_type == LITERAL_NULL)){
            operator->node->result_type = TYPE_BOOL_RESULT;
            operator->literal_type = NLITERAL_NULL;
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Compare null and nullable types variables
        if(((operatorType == EQUALITY) && (operand_first->literal_type == LITERAL_NULL) && (operand_second->literal_type == NLITERAL_STRING_NULL || operand_second->literal_type == NLITERAL_INT_NULL || operand_second->literal_type == NLITERAL_FLOAT_NULL))){
            operator->node->result_type = TYPE_BOOL_RESULT;
            operator->literal_type = NLITERAL_NULL;
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Compare nullable types variables and null
        if(((operatorType == EQUALITY) && (operand_second->literal_type == LITERAL_NULL) && (operand_first->literal_type == NLITERAL_STRING_NULL || operand_first->literal_type == NLITERAL_INT_NULL || operand_first->literal_type == NLITERAL_FLOAT_NULL))){
            operator->node->result_type = TYPE_BOOL_RESULT;
            operator->literal_type = NLITERAL_NULL;
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Compare two nullable variable and variable of same type(INT)
        if((operatorType == EQUALITY) && ((operand_first->literal_type == NLITERAL_INT_NULL && operand_second->literal_type == NLITERAL_INT) || (operand_first->literal_type == NLITERAL_INT && operand_second->literal_type == NLITERAL_INT_NULL))){
            operator->node->result_type = TYPE_BOOL_RESULT;
            operator->literal_type = NLITERAL_NULL;
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Compare variable and nullable variable(INT)
        if((operatorType == EQUALITY) && ((operand_first->literal_type == NLITERAL_INT && operand_second->literal_type == NLITERAL_INT_NULL) || (operand_first->literal_type == NLITERAL_INT_NULL && operand_second->literal_type == NLITERAL_INT))){
            operator->node->result_type = TYPE_BOOL_RESULT;
            operator->literal_type = NLITERAL_NULL;
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Compare two nullable variable and variable of same type(FLOAT)
        if((operatorType == EQUALITY) && ((operand_first->literal_type == NLITERAL_FLOAT_NULL && operand_second->literal_type == NLITERAL_FLOAT) || (operand_first->literal_type == NLITERAL_FLOAT && operand_second->literal_type == NLITERAL_FLOAT_NULL))){
            operator->node->result_type = TYPE_BOOL_RESULT;
            operator->literal_type = NLITERAL_NULL;
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Compare variable and nullable variable(FLOAT)
        if((operatorType == EQUALITY) && ((operand_first->literal_type == NLITERAL_FLOAT && operand_second->literal_type == NLITERAL_FLOAT_NULL) || (operand_first->literal_type == NLITERAL_FLOAT_NULL && operand_second->literal_type == NLITERAL_FLOAT))){
            operator->node->result_type = TYPE_BOOL_RESULT;
            operator->literal_type = NLITERAL_NULL;
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Result is nullable bool after retype of INT
        if((operatorType == EQUALITY) && ((operand_first->literal_type == LITERAL_INT && operand_second->literal_type == NLITERAL_FLOAT_NULL) || (operand_first->literal_type == NLITERAL_FLOAT_NULL && operand_second->literal_type == LITERAL_INT))){
            operator->node->result_type = TYPE_BOOL_RESULT;
            // Set type of literal
            operator->literal_type = NLITERAL_FLOAT;
            // Retype of INT to FLOAT
            if(operand_first->literal_type == LITERAL_INT) operand_first->node->convert_to_float = true;
            else operand_second->node->convert_to_float = true;
            // Delete two elements after active element, and move to next element
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }

        // Result is nullable bool after retype of FLOAT
        if((operatorType == EQUALITY) && ((operand_first->literal_type == LITERAL_FLOAT && operand_second->literal_type == NLITERAL_INT) || (operand_first->literal_type == NLITERAL_INT && operand_second->literal_type == LITERAL_FLOAT))){
            // Set type of subexpression
            operator->node->result_type = TYPE_BOOL_RESULT;
            // Set type of literal
            operator->literal_type = NLITERAL_NULL;
            // Retype of FLOAT to INT, only if the value of float has evrything after the decimal point 0
            if(operand_first->literal_type == LITERAL_FLOAT && is_float_int(operand_first->value)) operand_first->node->convert_to_int = true;
            else if(operand_second->literal_type == LITERAL_FLOAT && is_float_int(operand_second->value)) operand_second->node->convert_to_int = true;
            else{
                list_dispose(list_postfix);
                return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            }
            // Delete two elements after active element, and move to next element
            list_delete_two_after(list_postfix);
            list_next(list_postfix);
            continue;
        }
        

        list_dispose(list_postfix);
        return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;

    }
    

    // Return OK
    list_dispose(list_postfix);
    return RET_VAL_OK;

}

/**
 * @brief Function for adding function parameters to the symbol table
 * 
 * @param name Name of the function
 * @return int as defined in `RET_VAL` `return_values.h`
 * @retval 0=RET_VAL_OK added successfully
 * @retval 1=RET_VAL_INTERNAL_ERR failed
 */
int put_param_to_symtable(char *name) {
    if (name == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }
    
    T_SYMBOL *symbol = symtable_find_symbol(ST, name);
    if (symbol == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    T_SYMBOL_DATA data = symbol->data;

    if (data.func.argc == 0) {
        return RET_VAL_OK;
    } else {
        for (int i = 0; i < data.func.argc; i++) {
            T_SYMBOL_DATA sym_data;
            sym_data.var.type = data.func.argv[i].type;
            sym_data.var.is_const = true;
            sym_data.var.modified = true;
            sym_data.var.const_expr = false;
            sym_data.var.used = false;
            sym_data.var.id = -1;

            if (!symtable_add_symbol(ST, data.func.argv[i].name, SYM_VAR, sym_data)) {
                return RET_VAL_INTERNAL_ERR;
            }
        }
    }
    return RET_VAL_OK;
}
/**
 * @brief Function for checking if the result type is nullable
 * 
 * @param type Result type
 * @return bool
 * @retval true if the result type is nullable
 * @retval false if the result type is not nullable
 */
bool is_result_type_nullable(RESULT_TYPE type) {
    return type == TYPE_NULL_RESULT || type == TYPE_INT_NULL_RESULT || type == TYPE_FLOAT_NULL_RESULT || type == TYPE_STRING_NULL_RESULT;
}

/**
 * @brief Function for converting nullable result type to non-nullable
 * 
 * @param type Result type
 * @return VAR_TYPE
 */
VAR_TYPE fc_nullable_convert_type(RESULT_TYPE type) {
    switch (type) {
        case TYPE_NULL_RESULT:
            return VAR_NULL;
        case TYPE_INT_NULL_RESULT:
            return VAR_INT;
        case TYPE_FLOAT_NULL_RESULT:
            return VAR_FLOAT;
        case TYPE_STRING_NULL_RESULT:
            return VAR_STRING;
        default:
            return VAR_NONE;
    }
}