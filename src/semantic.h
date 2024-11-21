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