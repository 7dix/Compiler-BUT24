// FILE: main.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES:   Entry point of the IFJ24 compiler
//          Compiler is divided into two phases:
//          1.  Gather function signatures to Symtable, check that main exists
//              store all read tokens into a buffer
//          2. Use the buffer to run full syntax-based compilation

#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "first_phase.h"
#include "parser.h"
#include "token_buffer.h"
#include "symtable.h"
#include "gen_handler.h"

T_SYM_TABLE *ST;

int main() {

    // Initialize token buffer
    T_TOKEN_BUFFER *token_buffer = init_token_buffer();
    if (token_buffer == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    // Initialize symtable
    ST = symtable_init();
    if (ST == NULL) {
        free_token_buffer(&token_buffer);
        fprintf(stderr, "Error: Memory allocation failed in symtable_init\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Add global scope
    if (!symtable_add_scope(ST)) {
        free_token_buffer(&token_buffer);
        symtable_free(ST);
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_scope\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Run first phase of the compiler
    // first phase also fills symtable with built-in functions
    int error_code = first_phase(token_buffer);
    if (error_code != RET_VAL_OK) {
        // print error to stderr
        fprintf(stderr, "Error: First phase failed with error code %d\n", error_code);
        // in case of error, free res., return error code
        free_token_buffer(&token_buffer);
        symtable_free(ST);
        return error_code;
    }

    // set token buffer pointer to the first token
    set_current_to_first(token_buffer);

    // CD: generate ifj bytecode header, init global vars
    createProgramHeader();

    // Run second phase of the compiler
    error_code = run_parser(token_buffer);
    if (error_code != RET_VAL_OK) {
        // print error to stderr
        fprintf(stderr, "Error: Second phase failed with error code %d\n", error_code);
        // in case of error, free res., return error code
        free_token_buffer(&token_buffer);
        symtable_free(ST);
        return error_code;
    }

    free_token_buffer(&token_buffer);
    symtable_free(ST);

    return RET_VAL_OK;
}