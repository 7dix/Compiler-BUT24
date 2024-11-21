// FILE: semantic.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
// <Otakar Kočí> (xkocio00)
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

// get variable
Symbol *get_var(T_SYM_TABLE *table, const char *name) {
    Symbol *symbol = symtable_find_symbol(table, name);
    if (symbol == NULL) {
        return NULL;
    }
    if (symbol->type != SYM_VAR) {
        return NULL;
    }
    return symbol;
}