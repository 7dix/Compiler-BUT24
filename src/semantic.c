// FILE: semantic.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
// <Otakar Kočí> (xkocio00)
// Martin Zůbek (253206)
//
// YEAR: 2024
#include "semantic.h"


// check if the function call is valid
int check_function_call(T_SYM_TABLE *table, T_FN_CALL *fn_call) {
    Symbol *fn = symtable_find_symbol(table, fn_call->name);
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
                Symbol *symbol = symtable_find_symbol(table, fn_call->argv[i]->lexeme);
                if (symbol == NULL) {
                    return RET_VAL_SEMANTIC_UNDEFINED_ERR;
                }
                // check if the identifier is a variable
                if (symbol->type != SYM_VAR) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR; //TODO: correct return value?
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

// add argument to function call
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

// free function call arguments
void free_fn_call_args(T_FN_CALL *fn_call) {
    free(fn_call->argv);
}



// Compare variable types
int compare_var_types(VarType *existing, VarType *new) {
    // TODO: check all is correct
    if (*existing == *new) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_INT_NULL && *new == VAR_INT) {
        //*existing = VAR_INT;
        return RET_VAL_OK;
    }

    if (*existing == VAR_FLOAT_NULL && *new == VAR_FLOAT) {
        //*existing = VAR_FLOAT;
        return RET_VAL_OK;
    }

    if (*existing == VAR_STRING_NULL && *new == VAR_STRING) {
        //*existing = VAR_STRING;
        return RET_VAL_OK;
    }

    if (*existing == VAR_STRING && *new == STRING_VAR_STRING) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_STRING_NULL && *new == STRING_VAR_STRING) {
        //*existing = VAR_STRING;
        return RET_VAL_OK;
    }

    if (*new == VAR_BOOL) {
        return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
    }

    if (*new == VAR_ANY) {
        return RET_VAL_OK;
    }

    if (*existing == VAR_VOID) {
        if (*new == VAR_NULL) {
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
RetVal check_expression(T_SYM_TABLE *table, T_TREE_NODE_PTR *tree) {

    // Create and initialize list for postfix notation
    T_LIST_PTR listPostfix = list_init();

    // Check if list was created
    if(listPostfix == NULL) return RET_VAL_INTERNAL_ERR;
    // Postorder traversal, get list of nodes in postfix notation
    RetVal errExpr = postorder(tree, listPostfix);

    // Check if postorder traversal and creating list was successful
    if(errExpr){
        if(listPostfix->size > 0) list_dispose(listPostfix);
        return errExpr;
    }

    // Set types of operands
    errExpr = set_types(listPostfix, table);

    // Check compatibility of types
    if(errExpr){
        list_dispose(listPostfix);
        return errExpr;
    }
    // Set first element as active
    list_first(listPostfix);
   
    if(listPostfix->size == 1){
        switch (listPostfix->active->literalType){
            case LITERAL_INT:{ 
                (*tree)->resultType = TYPE_INT_RESULT;
                break;
            }
            case LITERAL_FLOAT:{ 
                (*tree)->resultType = TYPE_FLOAT_RESULT;
                break;
            }
            case LITERAL_NULL:{ 
                (*tree)->resultType = TYPE_NULL_RESULT;
                break;
            }
            case NLITERAL_INT:{ 
                (*tree)->resultType = TYPE_INT_RESULT;
                break;
            }
            case NLITERAL_FLOAT:{ 
                (*tree)->resultType = TYPE_FLOAT_RESULT;
                break;
            }
            case NLITERAL_INT_NULL:{ 
                (*tree)->resultType = TYPE_INT_NULL_RESULT;
                break;
            }
            case NLITERAL_FLOAT_NULL:{ 
                (*tree)->resultType = TYPE_FLOAT_NULL_RESULT;
                break;
            }
            case NELITERAL_STRING:{ 
                (*tree)->resultType = TYPE_STRING_RESULT;
                break;
            }
            case NELITERAL_STRING_NULL:{ 
                (*tree)->resultType = TYPE_STRING_NULL_RESULT;
                break;
            }

            default:{
                list_dispose(listPostfix);
                return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            }
        }

        list_dispose(listPostfix);
        return RET_VAL_OK;
    }

    if (listPostfix->size == 3 && (listPostfix->last->node->token->type == EQUAL || listPostfix->last->node->token->type == NOT_EQUAL)){
        T_LIST_ELEMENT_PTR operandOne = listPostfix->first;
        T_LIST_ELEMENT_PTR operandTwo = operandOne->next;
        

    
        if((operandOne->literalType == LITERAL_NULL) && (operandTwo->literalType == NELITERAL_STRING || operandTwo->literalType == NELITERAL_STRING_NULL || operandTwo->literalType == NLITERAL_INT_NULL || operandTwo->literalType == NLITERAL_FLOAT_NULL || operandTwo->literalType == NELITERAL_STRING)){
            (*tree)->resultType = TYPE_BOOL_RESULT;
            list_dispose(listPostfix);
            return RET_VAL_OK;
        }

        if((operandTwo->literalType == LITERAL_NULL) && (operandOne->literalType == NELITERAL_STRING || operandOne->literalType == NELITERAL_STRING_NULL || operandOne->literalType == NLITERAL_INT_NULL || operandOne->literalType == NLITERAL_FLOAT_NULL || operandOne->literalType == NELITERAL_STRING)){
            (*tree)->resultType = TYPE_BOOL_RESULT;
            list_dispose(listPostfix);
            return RET_VAL_OK;
        }

        if(operandOne->literalType == operandTwo->literalType && operandOne->literalType == LITERAL_NULL){
            (*tree)->resultType = TYPE_BOOL_RESULT;
            list_dispose(listPostfix);
            return RET_VAL_OK;
        }

        if(operandOne->literalType == operandTwo->literalType && operandOne->literalType == NLITERAL_INT_NULL){
            (*tree)->resultType = TYPE_BOOL_RESULT;
            list_dispose(listPostfix);
            return RET_VAL_OK;
        }

        if(operandOne->literalType == operandTwo->literalType && operandOne->literalType == NLITERAL_FLOAT_NULL){
            (*tree)->resultType = TYPE_BOOL_RESULT;
            list_dispose(listPostfix);
            return RET_VAL_OK;
        }

        if(operandOne->literalType == operandTwo->literalType && operandOne->literalType == NELITERAL_STRING_NULL){
            (*tree)->resultType = TYPE_BOOL_RESULT;
            list_dispose(listPostfix);
            return RET_VAL_OK;
        }

        if(operandOne->literalType == operandTwo->literalType && operandOne->literalType == NELITERAL_STRING){
            (*tree)->resultType = TYPE_BOOL_RESULT;
            list_dispose(listPostfix);
            return RET_VAL_OK;
        }
        
    }


    while(listPostfix->size != 1 && listPostfix->active != NULL){

        if(listPostfix->active->node->token->type == PLUS || listPostfix->active->node->token->type == MINUS || listPostfix->active->node->token->type == MULTIPLY || listPostfix->active->node->token->type == DIVIDE || listPostfix->active->node->token->type == LESS_THAN || listPostfix->active->node->token->type == GREATER_THAN || listPostfix->active->node->token->type == LESS_THAN_EQUAL || listPostfix->active->node->token->type == GREATER_THAN_EQUAL || listPostfix->active->node->token->type == EQUAL || listPostfix->active->node->token->type == NOT_EQUAL){

            T_LIST_ELEMENT_PTR operator = listPostfix->active;
            T_LIST_ELEMENT_PTR secondOperator = operator->prev;
            T_LIST_ELEMENT_PTR firstOperator = secondOperator->prev;

            // Check of division by zero
            if(operator->node->token->type == DIVIDE && (secondOperator->node->token->value.intVal == 0 ||  secondOperator->node->token->value.floatVal == 0.0)){
                list_dispose(listPostfix);
                return RET_VAL_SEMANTIC_OTHER_ERR;
                
            }
            
            
            
            // Result error of type compatibility
            if((firstOperator->literalType == LITERAL_INT && secondOperator->literalType == LITERAL_FLOAT) || (firstOperator->literalType == LITERAL_FLOAT && secondOperator->literalType == LITERAL_INT) || (firstOperator->literalType == NLITERAL_FLOAT && secondOperator->literalType == NLITERAL_INT) || (firstOperator->literalType == NLITERAL_INT && secondOperator->literalType == NLITERAL_FLOAT)){
                list_dispose(listPostfix);
                return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            }

            // Result NONLITERAL FLOAT
            if((firstOperator->literalType == NLITERAL_FLOAT && secondOperator->literalType == NLITERAL_FLOAT) || (firstOperator->literalType == NLITERAL_FLOAT && secondOperator->literalType == LITERAL_FLOAT) || (firstOperator->literalType == LITERAL_FLOAT && secondOperator->literalType == NLITERAL_FLOAT)){
                operator->literalType = NLITERAL_FLOAT;
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_FLOAT_RESULT;
                operator->literalType = NLITERAL_FLOAT;
                list_delete_two_after(listPostfix);
                list_next(listPostfix);
                continue;
            }
            
            // Result NONLITERAL INT
            if((firstOperator->literalType == NLITERAL_INT && secondOperator->literalType != NLITERAL_INT) || (firstOperator->literalType == NLITERAL_INT && secondOperator->literalType == LITERAL_INT) || (firstOperator->literalType == LITERAL_INT && secondOperator->literalType == NLITERAL_INT)){
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_INT_RESULT;
                operator->literalType = NLITERAL_INT;
                list_delete_two_after(listPostfix);
                list_next(listPostfix);
                continue;
            }

            // Result LITERAL INT
            if(firstOperator->literalType == LITERAL_INT && secondOperator->literalType == LITERAL_INT){
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_INT_RESULT;
                operator->literalType = LITERAL_INT;
                list_delete_two_after(listPostfix);
                list_next(listPostfix);
                continue;
            }

            // Result LITERAL FLOAT
            if(firstOperator->literalType == LITERAL_FLOAT && secondOperator->literalType == LITERAL_FLOAT){
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_FLOAT_RESULT;
                operator->literalType = LITERAL_FLOAT;
                list_delete_two_after(listPostfix);
                list_next(listPostfix);
                continue;
            }

            // Result NONLITERAL FLOAT after retype of INT
            if (firstOperator->literalType == LITERAL_INT && secondOperator->literalType == NLITERAL_FLOAT){
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_FLOAT_RESULT;
                operator->literalType = NLITERAL_FLOAT;
                // Retype of INT to FLOAT
                firstOperator->node->convertToFloat = true;
                list_delete_two_after(listPostfix);
                list_next(listPostfix);
                continue;
            }

            if (firstOperator->literalType== NLITERAL_FLOAT && secondOperator->literalType == LITERAL_INT){
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_FLOAT_RESULT;
                operator->literalType = NLITERAL_FLOAT;
                // Retype of INT to FLOAT
                secondOperator->node->convertToFloat = true;
                list_delete_two_after(listPostfix);
                list_next(listPostfix);
                continue;
            } 
            
            // Result NONLITERAL INT after retype of FLOAT
            if (firstOperator->literalType == NLITERAL_INT && secondOperator->literalType == LITERAL_FLOAT){
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_INT_RESULT;
                operator->literalType = NLITERAL_INT;
                // Retype of FLOAT to INT
                if (is_float_int(firstOperator->node->token->value.floatVal)){
                    firstOperator->node->convertToInt = true;
                    list_delete_two_after(listPostfix);
                    list_next(listPostfix);
                    continue;
                }else{
                    list_dispose(listPostfix);
                    return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
                }
                
            }

            if (firstOperator->literalType == LITERAL_FLOAT && secondOperator->literalType == NLITERAL_INT){
                if (operator->node->token->type == EQUAL || operator->node->token->type == NOT_EQUAL || operator->node->token->type == LESS_THAN || operator->node->token->type == GREATER_THAN || operator->node->token->type == LESS_THAN_EQUAL || operator->node->token->type == GREATER_THAN_EQUAL) operator->node->resultType = TYPE_BOOL_RESULT;
                else operator->node->resultType = TYPE_INT_RESULT;
                operator->literalType = NLITERAL_INT;
                // Retype of FLOAT to INT
                if (is_float_int(secondOperator->node->token->value.floatVal)){
                    secondOperator->node->convertToInt = true;
                    list_delete_two_after(listPostfix);
                    list_next(listPostfix);
                    continue;
                }else{
                    list_dispose(listPostfix);
                    return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
                }
                
            }
            
            // Error of type compatibility
            list_dispose(listPostfix);
            return RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            
        }else list_next(listPostfix);

    }

    list_dispose(listPostfix);
    return RET_VAL_OK;
}

// put parameter to symbol table
int put_param_to_symtable(char *name) {
    Symbol *symbol = symtable_find_symbol(ST, name);
    if (symbol == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    SymbolData data = symbol->data;

    if (data.func.argc == 0) {
        return RET_VAL_OK;
    } else {
        for (int i = 0; i < data.func.argc; i++) {
            SymbolData sym_data;
            sym_data.var.type = data.func.argv[i].type;
            sym_data.var.is_const = false;
            sym_data.var.modified = false;
            sym_data.var.used = false;
            sym_data.var.id = -1;

            if (!symtable_add_symbol(ST, data.func.argv[i].name, SYM_VAR, sym_data)) {
                return RET_VAL_INTERNAL_ERR;
            }
        }
    }
    return RET_VAL_OK;
}

bool is_result_type_nullable(RESULT_TYPE type) {
    return type == TYPE_NULL_RESULT || type == TYPE_INT_NULL_RESULT || type == TYPE_FLOAT_NULL_RESULT || type == TYPE_STRING_NULL_RESULT;
}

VarType fc_nullable_convert_type(RESULT_TYPE type) {
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
            return VAR_VOID;
    }
}