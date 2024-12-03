// FILE: generate.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES: Header file for the code generation instruction set for the IFJ24 language compiler.

#ifndef GENERATE_H
#define GENERATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtable.h"

// Function declarations
void handle_correct_string_format(char *input, char **output);
void generate_header();
void generate_move(char *frame, char *var, char *_frame, char *_var);
void generate_create_frame();
void generate_push_frame();
void generate_pop_frame();
void generate_defvar(char *frame, char *var);
void generate_call(char *label);
void generate_return();
void generate_pushs(char *frame, char *var);
void generate_pushs_int(int var);
void generate_pushs_float(float var);
void generate_pushs_string(char *var);
void generate_pops(char *frame, char *var);
void generate_clears();
void generate_add(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_sub(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_mul(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_div(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_idiv(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_adds();
void generate_subs();
void generate_muls();
void generate_divs();
void generate_idivs();
void generate_lt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_gt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_eq(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generate_lts();
void generate_gts();
void generate_eqs();
void generate_and(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generate_or(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generate_not(char *frame, char *var, char *_frame, char *_symb);
void generate_ands();
void generate_ors();
void generate_nots();
void generate_int2float(char *frame, char *var, char *_frame, char *_symb);
void generate_float2int(char *frame, char *var, char *_frame, char *_symb);
void generate_int2char(char *frame, char *var, char *_frame, char *_symb);
void generate_stri2int(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generate_int2floats();
void generate_float2ints();
void generate_int2chars();
void generate_stri2ints();
void generate_read(char *frame, char *var, char *type);
void generate_write(char *frame, char *var);
void generate_concat(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generate_strlen(char *frame, char *var, char *_frame, char *_symb);
void generate_getchar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generate_setchar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generate_type(char *frame, char *var, char *_frame, char *_symb);
void generate_label(char *label);
void generate_jump(char *label);
void generate_jumpifeq(char *label, char *frame, char *symb, char *_frame, char *_symb);
void generate_jumpifneq(char *label, char *frame, char *symb, char *_frame, char *_symb);
void generate_jumpifeqs(char *label);
void generate_jumpifneqs(char *label);
void generate_exit(int symb);
void generate_break();
void generate_dprint(char *frame, char *symb);

#endif // GENERATE_H
