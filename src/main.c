// FILE: main.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: Main function of the IFJ24 compiler

#include <stdio.h>
#include <stdlib.h>
#include "shared.h"
#include "first_phase.h"
#include "parser.h"
#include "token_buffer.h"

int main() {
    // TODO: stuff before

    // Initialize token buffer
    T_TOKEN_BUFFER *token_buffer = init_token_buffer();
    if (token_buffer == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    // TODO: init symtable
    // ...

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
        // TODO: free symtable as well
        return error_code;
    }

    // Run second phase of the compiler
    // TODO: give symtable to parser
    error_code = parser(token_buffer);
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
        // TODO: free symtable as well
    }
    
    // TODO: cleaning, ...

    return RET_VAL_OK;
}