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
#include "shared.h"
#include "token_buffer.h"
#include "symtable.h"
#include "semantic.h"
#include "precedence.h"
#include "precedence_tree.h"
#include "gen_handler.h"

// Function prototypes

int run_parser(T_TOKEN_BUFFER *token_buffer);
bool is_token_in_expr(T_TOKEN *token);
bool syntax_start(T_TOKEN_BUFFER *token_buffer);
bool syntax_prolog(T_TOKEN_BUFFER *buffer);
bool syntax_fn_defs(T_TOKEN_BUFFER *buffer);
bool syntax_fn_def(T_TOKEN_BUFFER *buffer);
bool syntax_fn_def_next(T_TOKEN_BUFFER *buffer);
bool syntax_fn_def_remaining(T_TOKEN_BUFFER *buffer);
bool syntax_params(T_TOKEN_BUFFER *buffer);
bool syntax_param(T_TOKEN_BUFFER *buffer);
bool syntax_type(T_TOKEN_BUFFER *buffer, VarType *type);
bool syntax_param_next(T_TOKEN_BUFFER *buffer);
bool syntax_param_after_comma(T_TOKEN_BUFFER *buffer);
bool syntax_end(T_TOKEN_BUFFER *buffer);
bool syntax_code_block_next(T_TOKEN_BUFFER *buffer);
bool syntax_code_block(T_TOKEN_BUFFER *buffer);
bool syntax_var_def(T_TOKEN_BUFFER *buffer);
bool syntax_var_def_after_id(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_if_statement(T_TOKEN_BUFFER *buffer);
bool syntax_if_statement_remaining(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree);
bool syntax_while_statement(T_TOKEN_BUFFER *buffer);
bool syntax_while_statement_remaining(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree);
bool syntax_return(T_TOKEN_BUFFER *buffer);
bool syntax_return_remaining(T_TOKEN_BUFFER *buffer);
bool syntax_built_in_void_fn_call(T_TOKEN_BUFFER *buffer);
bool syntax_assign_expr_or_fn_call(T_TOKEN_BUFFER *buffer);
bool syntax_assign_discard_expr_or_fn_call(T_TOKEN_BUFFER *buffer);
bool syntax_id_start(T_TOKEN_BUFFER *buffer, Symbol *symbol);
bool syntax_function_arguments(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_assign(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_id_assign(T_TOKEN_BUFFER *buffer);
bool syntax_arguments(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_argument_next(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_argument_after_comma(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_argument(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);

#endif // H_PARSER
