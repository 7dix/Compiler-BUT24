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
// TODO: include symtable when ready

T_SYM_TABLE *ST;

int main() {
    // TODO: stuff before ??

    // Initialize token buffer
    T_TOKEN_BUFFER *token_buffer = init_token_buffer();
    if (token_buffer == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    // Initialize symtable and add global scope
    ST = symtable_init();
    if (ST == NULL) {
        free_token_buffer(&token_buffer);
        fprintf(stderr, "Error: Memory allocation failed in symtable_init\n");
        return RET_VAL_INTERNAL_ERR;
    }
    if (!symtable_add_scope(ST)) {
        free_token_buffer(&token_buffer);
        symtable_free(ST);
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_scope\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Run first phase of the compiler
    // TODO: give first_phase symtable
    int error_code = first_phase(token_buffer);
    if (error_code != RET_VAL_OK) {
        switch (error_code) {
            case RET_VAL_LEXICAL_ERR:
                fprintf(stderr, "Lexical error\n");
                break;
            case RET_VAL_SYNTAX_ERR:
                fprintf(stderr, "Syntax error\n");
                break;
            case RET_VAL_INTERNAL_ERR:
                fprintf(stderr, "Internal error\n");
                break;
            default:
                fprintf(stderr, "Unknown error\n");
                break;
        }
        free_token_buffer(&token_buffer);
        symtable_free(ST);
        return error_code;
    }

    set_current_to_first(token_buffer);

    // Run second phase of the compiler
    // TODO: give symtable to parser
    error_code = run_parser(token_buffer);
    if (error_code != RET_VAL_OK) {
        switch (error_code) {
            case RET_VAL_LEXICAL_ERR:
                fprintf(stderr, "Lexical error\n");
                break;
            case RET_VAL_SYNTAX_ERR:
                fprintf(stderr, "Syntax error\n");
                break;
            case RET_VAL_INTERNAL_ERR:
                fprintf(stderr, "Internal error\n");
                break;
            // TODO: add more error codes, semantics mostly
            default:
                fprintf(stderr, "Unknown error\n");
                break;
        }
        free_token_buffer(&token_buffer);
        symtable_free(ST);
        return error_code;
    }

    // TODO: remove later, just for testing
    fprintf(stdout, "Compilation successful\n");

    // TODO: cleaning, ...

    free_token_buffer(&token_buffer);
    symtable_free(ST);

    return RET_VAL_OK;
}