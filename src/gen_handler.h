// FILE: gen_handler.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES: Header file for gen_handler.c

#ifndef GEN_HANDLER_H
#define GEN_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "generate.h"
#include "gen_handler.h"
#include "precedence_tree.h"
#include "symtable.h"
#include "semantic.h"

// Function declarations
void create_program_header();
void generate_unique_identifier(char *name, char **uniq_name);
void create_fn_header(char *name);
void call_function(T_FN_CALL *fn);
void create_return();
void handle_discard();
void handle_uniq_defvar(T_TOKEN *var);
void solve_exp_by_postorder(T_TREE_NODE *tree);
void handle_assign(char *var);
void call_bi_readint();
void call_bi_readfloat();
void call_bi_readstring();
void call_bi_int2float(T_TOKEN *var);
void call_bi_float2int(T_TOKEN *var);
void call_bi_string(T_TOKEN *var);
void call_bi_length(T_TOKEN *var);
void call_bi_concat(T_TOKEN *var, T_TOKEN *_var);
void call_bi_substring(T_TOKEN *var, T_TOKEN *beg, T_TOKEN *end);
void call_bi_strcmp(T_TOKEN *var, T_TOKEN *_var);
void call_bi_ord(T_TOKEN *var, T_TOKEN *index);
void call_bi_chr(T_TOKEN *var);
void call_bi_write(T_TOKEN *var);
void call_bi_fn(T_FN_CALL *fn);
void handle_if_start_bool(char *label_else, int upper, int current);
void handle_if_start_nil(char *label_else, T_TOKEN *var, int upper, int current);
void create_if_else(char *label_end, char *label_else, int upper, int current_if, int current_else);
void create_if_end(char *label_end, int current);
void create_while_bool_header(char *label_start, int upper, int current);
void create_while_nil_header(char *label_start, T_TOKEN *var, int upper, int current);
void handle_while_bool(char *label_end);
void handle_while_nil(char *label_end, T_TOKEN *var);
void create_while_end(char *label_start, char *label_end, int while_def_counter);

#endif // GEN_HANDLER_H

