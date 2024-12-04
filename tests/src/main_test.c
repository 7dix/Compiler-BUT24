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
#include "../../src/return_values.h"
#include "../../src/first_phase.h"
#include "../../src/parser.h"
#include "../../src/token_buffer.h"
#include "../../src/symtable.h"
// TODO: include symtable when ready

T_SYM_TABLE *ST;

int main(int argc, char *argv[]) {
    // TODO: stuff before ??

    if (argc < 2) {
        fprintf(stderr, "Error: No input file provided\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Load file into stdin
    if (freopen(argv[1], "r", stdin) == NULL) {
        fprintf(stderr, "Error: Could not open input file %s\n", argv[1]);
        return RET_VAL_INTERNAL_ERR;
    }

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
    if (!symtable_add_scope(ST, false)) {
        free_token_buffer(&token_buffer);
        symtable_free(ST);
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_scope\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Run first phase of the compiler
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
            case RET_VAL_SEMANTIC_UNDEFINED_ERR:
                fprintf(stderr, "Semantic error: Undefined function 'main'\n");
                break;
            case RET_VAL_SEMANTIC_FUNCTION_ERR:
                fprintf(stderr, "Semantic error: Function 'main' has wrong return type or arguments\n");
                break;
            case RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR:
                fprintf(stderr, "Semantic error: Function 'main' redefined or assigned to variable\n");
                break;
            case RET_VAL_SEMANTIC_FUNC_RETURN_ERR:
                fprintf(stderr, "Semantic error: Function 'main' has wrong return type or arguments\n");
                break;
            case RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR:
                fprintf(stderr, "Semantic error: Function 'main' has wrong return type or arguments\n");
                break;
            case RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR:
                fprintf(stderr, "Semantic error: Function 'main' has wrong return type or arguments\n");
                break;
            case RET_VAL_SEMANTIC_UNUSED_VAR_ERR:
                fprintf(stderr, "Semantic error: Function 'main' has wrong return type or arguments\n");
                break;
            case RET_VAL_SEMANTIC_OTHER_ERR:
                fprintf(stderr, "Semantic error\n");
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
    int line = token_buffer->curr->token->line;
    if (error_code != RET_VAL_OK) {
        switch (error_code) {
            case RET_VAL_LEXICAL_ERR:
                fprintf(stderr, "Lexical error. Line: %d\n", line);
                break;
            case RET_VAL_SYNTAX_ERR:
                fprintf(stderr, "Syntax error. Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_UNDEFINED_ERR:
                fprintf(stderr, "Semantic error: Undefined function or variable. Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_FUNCTION_ERR:
                fprintf(stderr, "Semantic error: Function call (wrong type or number of params), return value (wrong type or not allowed discard). Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR:
                fprintf(stderr, "Semantic error: Redefinition of variable or function, assignment to constant. Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_FUNC_RETURN_ERR:
                fprintf(stderr, "Semantic error: Missing or too many expressions in function return. Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR:
                fprintf(stderr, "Semantic error: Wrong type compatibility in arithmetic, string and/or relation expression, incompatible type during assignment. Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR:
                fprintf(stderr, "Semantic error: Type is not stated and cannot be derived from the expression. Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_UNUSED_VAR_ERR:
                fprintf(stderr, "Semantic error: Unused variable, modifiable variable without possibility of change after initialization. Line: %d\n", line);
                break;
            case RET_VAL_SEMANTIC_OTHER_ERR:
                fprintf(stderr, "Semantic error. Line: %d\n", line);
                break;
            case RET_VAL_INTERNAL_ERR:
                fprintf(stderr, "Internal error. Line: %d\n", line);
                break;
            default:
                fprintf(stderr, "Unknown error. Line: %d\n", line);
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