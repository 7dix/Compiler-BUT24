// FILE: parser.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: Header file for the parser

#ifndef H_PARSER
#define H_PARSER

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "shared.h"

// Function prototypes
int run_parser();
bool is_token_in_expr();
bool syntax_start();
bool syntax_prolog();
bool syntax_fn_defs();
bool syntax_fn_def();
bool syntax_fn_def_next();
bool syntax_fn_def_remaining();
bool syntax_params();
bool syntax_param();
bool syntax_type();
bool syntax_param_next();
bool syntax_param_after_comma();
bool syntax_end();
bool syntax_code_block_next();
bool syntax_code_block();
bool syntax_var_def();
bool syntax_var_def_after_id();
bool syntax_if_statement();
bool syntax_if_statement_remaining();
bool syntax_while_statement();
bool syntax_while_statement_remaining();
bool syntax_return();
bool syntax_return_remaining();
bool syntax_built_in_void_fn_call();
bool syntax_assign_expr_or_fn_call();
bool syntax_assign_discard_expr_or_fn_call();
bool syntax_id_start();
bool syntax_function_arguments();
bool syntax_assign();
bool syntax_id_assign();
bool syntax_arguments();
bool syntax_argument_next();
bool syntax_argument_after_comma();
bool syntax_argument();

#endif // H_PARSER
