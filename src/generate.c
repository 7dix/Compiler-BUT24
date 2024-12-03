// FILE: generator.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES: Code generation instruction set for the IFJ24 language compiler.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symtable.h"

// Label counter definition
int label_counter = 0;

/***********************************************************************
 *                  CONVERSION TO PRINTABLE STRING
 ***********************************************************************
*/
 
void handle_correct_string_format(char *input, char **output) {
    *output = (char *)malloc(4 * strlen(input) + 1); // Allocate enough memory for the worst case
    *output[0] = '\0'; // Init the output string

    while (*input) {
        char current = *input;
        char next = *(input + 1);
        
        if (current == 35 || current == 92 || current <= 32) { // if the char is #, \ or a control char
            char hex[6];

            if (current == '\\' && next == 'n') { // Special newline case
                sprintf(hex, "\\010");
                input++; // Skip the 'n'
            }
            else if (current == '\\' && next == 't') { // Special tab case
                sprintf(hex, "\\009");
                input++; // Skip the 't'
            }
            else if (current == '\\' && next == 'r') { // Special carriage return case
                sprintf(hex, "\\013");
                input++; // Skip the 'r'
            }
            else if (current == '\\' && next == '0') {
                sprintf(hex, "\\000"); // Special null case
                input++; // Skip the '0'
            }
            else {
                sprintf(hex, "\\%03d", current); // Other cases
            }
            strcat(*output, hex);
        }
        else {
            size_t len = strlen(*output);
            (*output)[len] = current;
            (*output)[len + 1] = '\0'; // Always null-terminate the string
        }
        input++;
    }
}

/***********************************************************************
 *                  INSTRUCTION GENERATIVE FUNCTIONS
 ***********************************************************************
 * Functions that generate the IFJ24 language instructions to call in the
 * interpreter.
*/

void generate_header() {
    printf(".IFJcode24\n");
}

void generate_move(char *frame, char *var, char *_frame, char *_var) {
    printf("MOVE %s@%s %s@%s\n", frame, var, _frame, _var);
}

void generate_create_frame() {
    printf("CREATEFRAME\n");
}

void generate_push_frame() {
    printf("PUSHFRAME\n");
}

void generate_pop_frame() {
    printf("POPFRAME\n");
}

void generate_defvar(char *frame, char *var) {
    int fcDefId;
    fcDefId = is_in_fc(ST);

    if (fcDefId >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@defined$%d bool@true\n", label_counter, fcDefId);
        printf("DEFVAR LF@%s\n", var);
        printf("LABEL skipDefvar$%d\n", label_counter);
        label_counter++;
    }
    else {
        printf("DEFVAR %s@%s\n", frame, var);
    }
}

void generate_call(char *label) {
    printf("CALL %s\n", label);
}

void generate_return() {
    printf("RETURN\n");
}

void generate_pushs(char *frame, char *var) {
    printf("PUSHS %s@%s\n", frame, var);
}

void generate_pushs_int(int var) {
    printf("PUSHS int@%d\n", var);
}

void generate_pushs_float(float var) {
    printf("PUSHS float@%a\n", var);
}

void generate_pushs_string(char *var) {
    char *out = NULL;
    handle_correct_string_format(var, &out);
    printf("PUSHS string@%s\n", out);
    free(out);
}

void generate_pops(char *frame, char *var) {
    printf("POPS %s@%s\n", frame, var);
}

void generate_clears() {
    printf("CLEARS\n");
}

void generate_add(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("ADD %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_sub(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("SUB %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_mul(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("MUL %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_div(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("DIV %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_idiv(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("IDIV %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_adds() {
    printf("ADDS\n");
}

void generate_subs() {
    printf("SUBS\n");
}

void generate_muls() {
    printf("MULS\n");
}

void generate_divs() {
    printf("DIVS\n");
}

void generate_idivs() {
    printf("IDIVS\n");
}

void generate_lt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("LT %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_gt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("GT %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_eq(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("EQ %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generate_lts() {
    printf("LTS\n");
}

void generate_gts() {
    printf("GTS\n");
}

void generate_eqs() {
    printf("EQS\n");
}

void generate_and(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("AND %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generate_or(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("OR %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generate_not(char *frame, char *var, char *_frame, char *_symb) {
    printf("NOT %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generate_ands() {
    printf("ANDS\n");
}

void generate_ors() {
    printf("ORS\n");
}

void generate_nots() {
    printf("NOTS\n");
}

void generate_int2float(char *frame, char *var, char *_frame, char *_symb) {
    printf("INT2FLOAT %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generate_float2int(char *frame, char *var, char *_frame, char *_symb) {
    printf("FLOAT2INT %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generate_int2char(char *frame, char *var, char *_frame, char *_symb) {
    printf("INT2CHAR %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generate_stri2int(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("STRI2INT %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generate_int2floats() {
    printf("INT2FLOATS\n");
}

void generate_float2ints() {
    printf("FLOAT2INTS\n");
}

void generate_int2chars() {
    printf("INT2CHARS\n");
}

void generate_stri2ints() {
    printf("STRI2INTS\n");
}

void generate_read(char *frame, char *var, char *type) {
    if (strcmp(type, "int") == 0) {
        printf("READ %s@%s int\n", frame, var);
    }

    else if (strcmp(type, "float") == 0) {
        printf("READ %s@%s float\n", frame, var);
    }

    else if (strcmp(type, "string") == 0) {
        printf("READ %s@%s string\n", frame, var);
    }

    else if (strcmp(type, "bool") == 0) {
        printf("READ %s@%s bool\n", frame, var);
    }

    else{
        exit(99);
    }
}

void generate_write(char *frame, char *var) {
    printf("WRITE %s@%s\n", frame, var);
}

void generate_concat(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("CONCAT %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generate_strlen(char *frame, char *var, char *_frame, char *_symb) {
    printf("STRLEN %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generate_getchar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("GETCHAR %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generate_setchar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("SETCHAR %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generate_type(char *frame, char *var, char *_frame, char *_symb) {
    printf("TYPE %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generate_label(char *label) {
    printf("LABEL %s\n", label);
}

void generate_jump(char *label) {
    printf("JUMP %s\n", label);
}

void generate_jumpifeq(char *label, char *frame, char *symb, char *_frame, char *_symb) {
    printf("JUMPIFEQ %s %s@%s %s@%s\n", label, frame, symb, _frame, _symb);
}

void generate_jumpifneq(char *label, char *frame, char *symb, char *_frame, char *_symb) {
    printf("JUMPIFNEQ %s %s@%s %s@%s\n", label, frame, symb, _frame, _symb);
}

void generate_jumpifeqs(char *label) {
    printf("JUMPIFEQS %s\n", label);
}

void generate_jumpifneqs(char *label) {
    printf("JUMPIFNEQS %s\n", label);
}

void generate_exit(int symb) {
    printf("EXIT int@%d\n", symb);
}

void generate_break() {
    printf("BREAK\n");
}

void generate_dprint(char *frame, char *symb) {
    printf("DPRINT %s@%s\n", frame, symb);
}

/** End of the instruction generative functions */