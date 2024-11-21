// FILE: semantic.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
// <Otakar Kočí> (xkocio00)
//
// YEAR: 2024

#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"
#include "scanner.h"
#include "shared.h"

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