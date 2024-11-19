// FILE: first_phase.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: header file for the first phase of the compiler

#ifndef FIRST_PHASE_H
#define FIRST_PHASE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "token_buffer.h"
#include "symtable.h"
#include "scanner.h"
#include "shared.h"

// Function prototypes
int first_phase(T_TOKEN_BUFFER *token_buffer);
bool get_save_token(T_TOKEN_BUFFER *token_buffer, T_TOKEN **token);
bool syntax_fp_start(T_TOKEN_BUFFER *buffer);
bool syntax_fp_prolog(T_TOKEN_BUFFER *buffer);
bool syntax_fp_fn_defs(T_TOKEN_BUFFER *buffer);
bool syntax_fp_fn_def(T_TOKEN_BUFFER *buffer);
bool syntax_fp_fn_def_next(T_TOKEN_BUFFER *buffer);
bool syntax_fp_fn_def_remaining(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_fp_type(T_TOKEN_BUFFER *buffer, VarType *type);
bool syntax_fp_params(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_fp_param(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_fp_param_next(T_TOKEN_BUFFER *buffe, SymbolData *data);
bool syntax_fp_param_after_comma(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_fp_end(T_TOKEN_BUFFER *buffer);
bool simulate_fn_body(T_TOKEN_BUFFER *buffer);


#endif // FIRST_PHASE_H