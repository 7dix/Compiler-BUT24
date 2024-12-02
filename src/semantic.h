// FILE: semantic.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//  <Martin Zůbek> (253206)
// 
// YEAR: 2024
// NOTES: Header file for module with helper functions for semantic analysis

#ifndef H_SEMANTIC
#define H_SEMANTIC

#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"
#include "scanner.h"
#include "return_values.h"
#include "precedence_tree.h"
#include "semantic_list.h"

typedef struct FN_CALL {
    char *name;
    VAR_TYPE ret_type;
    int argc;
    T_TOKEN **argv;
} T_FN_CALL;

// function prototypes

int check_function_call(T_SYM_TABLE *table, T_FN_CALL *fn_call);
int add_arg_to_fn_call(T_FN_CALL *fn_call, T_TOKEN *arg);
void free_fn_call_args(T_FN_CALL *fn_call);
T_SYMBOL *get_var(T_SYM_TABLE *table, const char *name);
RET_VAL check_expression(T_SYM_TABLE *table, T_TREE_NODE_PTR * tree);
int put_param_to_symtable(char *name);

// compare variable types
int compare_var_types(VAR_TYPE *existing, VAR_TYPE *new);
bool is_result_type_nullable(RESULT_TYPE type);
VAR_TYPE fc_nullable_convert_type(RESULT_TYPE type);

#endif // H_SEMANTIC
