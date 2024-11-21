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
                if (symbol->data.var.type != fn->data.func.argv[i].type) {
                    return RET_VAL_SEMANTIC_FUNCTION_ERR;
                }
                break;
            
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

// Checks for unused and unmodified variables in the symbol table
int check_for_unused_vars(T_SYM_TABLE *table) {
    if (table == NULL || table->top == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    Hashtable *ht = table->top->ht;
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        if (ht->table[i].occupied && ht->table[i].type == SYM_VAR) {
            if (!ht->table[i].data.var.used || !ht->table[i].data.var.modified) {
                return RET_VAL_SEMANTIC_UNUSED_VAR_ERR;
            }
        }
    }

    return RET_VAL_OK;
}

/**
 * @brief Function for semantic analysis of expression
 * @param table Pointer to the symbol table
 * @param tree Pointer to the root of the tree
 * @return 0=RET_VAL_OK if the expression is valid, otherwise return one of semnatic errors
 */
RetVal check_expression(T_SYM_TABLE *table, T_TREE_NODE_PTR *tree) {
    // Check internal failure
    if (tree == NULL || table == NULL) return RET_VAL_INTERNAL_ERR;

    // Create list for postfix notation
    T_LIST_PTR listPostfix = NULL;
    // Initialize list
    list_init(listPostfix);
    // Get postfix notation from tree to list
    if(list_get_postfix_notation(listPostfix, tree)) return RET_VAL_INTERNAL_ERR;



    // Set literal type of every element in list
    if(set_literal_type(table, listPostfix)){
        list_dispose(listPostfix);
        return RET_VAL_SEMANTIC_UNDEFINED_ERR;
    }


    // TODO: IMPLEMENT FOR ONE OPERAND IN EXPRESSION

    // Until list has only one element, sumulating of operation
    while(listPostfix->size != 1){
        // Get first three elements from list, operator is always binary
        T_LIST_ELEMENT_PTR operandOne = listPostfix->first;
        T_LIST_ELEMENT_PTR operandTwo = operandOne->next;
        T_LIST_ELEMENT_PTR operator = operandTwo->next;
        T_TREE_NODE_PTR lastResult = NULL;

        if((operandOne->literalType == NLITERAL_INT && operandTwo == NLITERAL_FLOAT) || (operandOne->literalType == NLITERAL_FLOAT && operandTwo == NLITERAL_INT)){
            list_dispose(listPostfix);
            return RET_VAL_SEMANTIC_TYPE_ERR;
        }

        if ((operandOne->literalType == LITERAL_INT && operandTwo->literalType == LITERAL_FLOAT) || (operandOne->literalType == LITERAL_FLOAT && operandTwo->literalType == LITERAL_INT)){
            list_dispose(listPostfix);
            return RET_VAL_SEMANTIC_TYPE_ERR;
        }

        if (operandOne->literalType == LITERAL_INT_NLL || operandTwo->literalType == LITERAL_INT_NLL){
            list_dispose(listPostfix);
            return RET_VAL_SEMANTIC_TYPE_ERR;
        }
    }
              
    return RET_VAL_OK;
}