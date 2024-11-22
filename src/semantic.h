// FILE: semantic.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
// <Otakar Kočí> (xkocio00)
// Martin Zůbek (253206)
// 
// YEAR: 2024

#ifndef H_SEMANTIC
#define H_SEMANTIC

#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"
#include "scanner.h"
#include "shared.h"
#include "precedence_tree.h"
#include "semantic_list.h"

typedef struct FN_CALL {
    char *name;
    VarType ret_type;
    int argc;
    T_TOKEN **argv;
} T_FN_CALL;

// function prototypes

int check_function_call(T_SYM_TABLE *table, T_FN_CALL *fn_call);
int add_arg_to_fn_call(T_FN_CALL *fn_call, T_TOKEN *arg);
void free_fn_call_args(T_FN_CALL *fn_call);
Symbol *get_var(T_SYM_TABLE *table, const char *name);
RetVal check_expression(T_SYM_TABLE *table, T_TREE_NODE_PTR * tree);
int put_param_to_symtable(char *name);

// compare variable types
int compare_var_types(VarType *existing, VarType *new);

#endif // H_SEMANTIC
