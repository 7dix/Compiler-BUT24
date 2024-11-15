// FILE: parser.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: Parser for the IFJ24 language

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "shared.h"

T_TOKEN token;
int error_flag = RET_VAL_OK;

// those are just dummy functions, later on we will probably use buffer
// (doubly linked list) to store tokens
void next_token() {
    get_token(&token);
}

void token_buffer_move_back() {
    return;
}

/**
 * @brief Entry point of the parser.
 * 
 * @note TODO: add semantic checks, cleaning, etc.
 * @note TODO: change signature to work properly with future buffer
 * @note TODO: improve comments on return values
 * @return 
 * - RET_VAL_OK if parsing was successful
 * 
 * - RET_VAL_SYNTAX_ERR if syntax error occurred
 */
int run_parser() {
    // TODO: add semantic checks, cleaning, etc.
    // Start of recursive parser
    if (!syntax_start()) {
        return error_flag;
    }
    return RET_VAL_OK;
}

/**
 * @brief Checks whether given token can occur in expression.
 * 
 * This function uses following global variables:
 * 
 * - T_TOKEN `token`
 * @return
 * - `true` if token can occur in expression
 * 
 * - `false` otherwise
 */
bool is_token_in_expr() {
    bool is_in_expr = ( token.type == NOT_EQUAL || token.type == INT ||
                        token.type == FLOAT || token.type == LESS_THAN ||
                        token.type == GREATER_THAN || token.type == EQUAL ||
                        token.type == LESS_THAN_EQUAL || token.type == GREATER_THAN_EQUAL ||
                        token.type == PLUS || token.type == MINUS || token.type == MULTIPLY ||
                        token.type == DIVIDE || token.type == BRACKET_LEFT_SIMPLE ||
                        token.type == BRACKET_RIGHT_SIMPLE || token.type == IDENTIFIER );
    return is_in_expr;
}

/**
 * @brief Start of recursive parser. Simulates START non-terminal.
 * 
 * START is defined as:
 * START -> PROLOG FN_DEFS END
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_start() {
    // TODO: add semantic checks, cleaning, etc.

    if (!syntax_prolog()) {
        return false;
    }
    if (!syntax_fn_defs()) {
        return false;
    }
    if (!syntax_end()) {
        return false;
    }
    return true;
}

/**
 * @brief Checks Syntax of prolog. Simulates PROLOG non-terminal.
 * 
 * PROLOG is defined as:
 * PROLOG -> const ifj = @import("ifj24.zig");
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_prolog() {
    // TODO: add semantic checks, cleaning, etc.
    
    next_token(); // const
    if (token.type != CONST) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(); // ifj
    if (token.type != IFJ) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(); // =
    if (token.type != ASSIGN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    next_token(); // @import
    if (token.type != IMPORT) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    next_token(); // (
    if (token.type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(); // string
    if (token.type != STRING) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // check that string is equal to "ifj24.zig"
    if (strcmp(token.lexeme, "ifj24.zig") != 0) {
        // TODO: process error
        // TODO: is this syntax or semantic error?
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(); // )
    if (token.type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(); // ;
    if (token.type != SEMICOLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    return true;
}

/**
 * @brief Checks Syntax of function definitions. Simulates FN_DEFS non-terminal.
 * 
 * FN_DEFS is defined as:
 * FN_DEFS -> FN_DEF FN_DEF_NEXT
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_fn_defs() {
    // TODO: add semantic checks, cleaning, etc.

    if (!syntax_fn_def()) {
        return false;
    }
    if (!syntax_fn_def_next()) {
        return false;
    }
    return true;
}

/**
 * @brief Checks Syntax of function definition. Simulates FN_DEF non-terminal.
 * 
 * FN_DEF is defined as:
 * FN_DEF -> pub fn identifier ( PARAMS ) FN_DEF_REMAINING
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_fn_def() {
    // TODO: add semantic checks, cleaning, etc.

    // pub
    next_token();
    if (token.type != PUB) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // fn
    next_token();
    if (token.type != FN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // identifier
    next_token();
    if (token.type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // (
    next_token();
    if (token.type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal PARAMS
    // not checking token here
    if (!syntax_params()) {
        return false;
    }

    // )
    next_token();
    if (token.type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal FN_DEF_REMAINING
    // not checking token here
    if (!syntax_fn_def_remaining()) {
        return false;
    }

    // All passed
    return true;
}

/**
 * @brief Checks Syntax of following function definitions. Simulates FN_DEF_NEXT non-terminal.
 * 
 * FN_DEF_NEXT is defined as:
 * FN_DEF_NEXT -> ε
 * FN_DEF_NEXT -> FN_DEF FN_DEF_NEXT
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_fn_def_next() {
    // TODO: add semantic checks, cleaning, etc.
    
    // we have two possible branches, need to choose here
    next_token();
    token_buffer_move_back(); // token needed in both branches
    // first branch -> ε
    if (token.type == EOF_TOKEN) {
        return true;
    }

    // second branch -> FN_DEF FN_DEF_NEXT
    if (token.type == PUB) { // pub is first terminal in FN_DEF
        if (!syntax_fn_def()) {
            return false;
        }
        if (!syntax_fn_def_next()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks Syntax of the rest of the fn def. Simulates FN_DEF_REMAINING non-terminal.
 * 
 * FN_DEF_REMAINING is defined as:
 * FN_DEF_REMAINING -> TYPE { CODE_BLOCK_NEXT }
 * FN_DEF_REMAINING -> void { CODE_BLOCK_NEXT }
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_fn_def_remaining() {
    // TODO: add semantic checks, cleaning, etc.
    // TODO: possible simplification by checking only void type

    // to be checked in syntax_type function
    // we have two possible branches, need to choose here
    next_token();
    // first branch -> TYPE { CODE_BLOCK_NEXT }, return token
    if (token.type == TYPE_INT || token.type == TYPE_FLOAT ||
        token.type == TYPE_STRING || token.type == TYPE_INT_NULL ||
        token.type == TYPE_FLOAT_NULL || token.type == TYPE_STRING_NULL) {
        // follows non-terminal TYPE
        token_buffer_move_back();
        if (!syntax_type()) {
            return false;
        }

        // {
        next_token();
        if (token.type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal CODE_BLOCK_NEXT
        // not checking token here
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> void { CODE_BLOCK_NEXT }
    if (token.type == VOID) {
        // {
        next_token();
        if (token.type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal CODE_BLOCK_NEXT
        // not checking token here
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks Syntax of function parameters. Simulates PARAMS non-terminal.
 * 
 * PARAMS is defined as:
 * PARAMS -> ε
 * PARAMS -> PARAM PARAM_NEXT
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_params() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    token_buffer_move_back(); // token needed in both branches
    // first branch -> ε
    if (token.type == BRACKET_RIGHT_SIMPLE) {
        return true;
    }
    // second branch -> PARAM PARAM_NEXT
    if (token.type == IDENTIFIER) { // identifier is first terminal in PARAM
        if (!syntax_param()) {
            return false;
        }
        if (!syntax_param_next()) {
            return false;
        }

        return true;
    }
    
    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks Syntax of function parameter. Simulates PARAM non-terminal.
 * 
 * PARAM is defined as:
 * PARAM -> identifier : TYPE
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_param() {
    // TODO: add semantic checks, cleaning, etc.

    // identifier
    next_token();
    if (token.type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // :
    next_token();
    if (token.type != COLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal TYPE
    // not checking token here
    if (!syntax_type()) {
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of type definition. Simulates TYPE non-terminal.
 * 
 * TYPE is defined as:
 * TYPE -> type_int
 * TYPE -> type_float
 * TYPE -> type_string
 * TYPE -> type_int_null
 * TYPE -> type_float_null
 * TYPE -> type_string_null
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_type() {
    // TODO: add semantic checks, cleaning, etc.

    // check whether token is one of the types
    next_token();
    if (token.type != TYPE_INT && token.type != TYPE_FLOAT &&
        token.type != TYPE_STRING && token.type != TYPE_INT_NULL && 
        token.type != TYPE_FLOAT_NULL && token.type != TYPE_STRING_NULL) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of following function params. Simulates PARAM_NEXT non-terminal.
 * 
 * PARAM_NEXT is defined as:
 * PARAM_NEXT -> ε
 * PARAM_NEXT -> , PARAM_AFTER_COMMA
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_param_next() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> ε, we need to return the token
    if (token.type == BRACKET_RIGHT_SIMPLE) {
        token_buffer_move_back();
        return true;
    }

    // second branch -> , PARAM_AFTER_COMMA, consuming token
    if (token.type == COMMA) { // ,
        if (!syntax_param_after_comma()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of following function params after comma. Simulates PARAM_AFTER_COMMA non-terminal.
 * 
 * PARAM_AFTER_COMMA is defined as:
 * PARAM_AFTER_COMMA -> ε
 * PARAM_AFTER_COMMA -> PARAM PARAM_NEXT
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_param_after_comma() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    token_buffer_move_back(); // token needed in both branches
    // first branch -> ε
    if (token.type == BRACKET_RIGHT_SIMPLE) {
        return true;
    }

    // second branch -> PARAM PARAM_NEXT
    if (token.type == IDENTIFIER) { // identifier is first terminal in PARAM
        if (!syntax_param()) {
            return false;
        }
        if (!syntax_param_next()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks end of file. Simulates END non-terminal.
 * 
 * END is defined as:
 * END -> eof_token
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_end() {
    // check last terminal in the program -> EOF
    next_token();
    if (token.type != EOF_TOKEN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    return true;
}

/**
 * @brief Checks syntax of code block. Simulates CODE_BLOCK_NEXT non-terminal.
 * 
 * CODE_BLOCK_NEXT is defined as:
 * CODE_BLOCK_NEXT -> ε
 * CODE_BLOCK_NEXT -> CODE_BLOCK CODE_BLOCK_NEXT
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_code_block_next() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    token_buffer_move_back(); // token needed in both branches
    // first branch -> ε
    if (token.type == BRACKET_RIGHT_CURLY) {
        return true;
    }

    // second branch -> CODE_BLOCK CODE_BLOCK_NEXT
    if (token.type == CONST || token.type == VAR || token.type == IF ||
        token.type == WHILE || token.type == RETURN || token.type == IFJ ||
        token.type == IDENTIFIER || token.type == IDENTIFIER_DISCARD) {
        if (!syntax_code_block()) {
            return false;
        }
        if (!syntax_code_block_next()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates CODE_BLOCK non-terminal.
 * 
 * CODE_BLOCK is defined as:
 * 
 * - `CODE_BLOCK -> VAR_DEF` 
 * 
 * - CODE_BLOCK -> IF_STATEMENT 
 * 
 * - CODE_BLOCK -> WHILE_STATEMENT 
 * 
 * - CODE_BLOCK -> RETURN 
 * 
 * - CODE_BLOCK -> ASSIGN_EXPR_OR_FN_CALL 
 * 
 * - CODE_BLOCK -> ASSIGN_DISCARD_EXPR_OR_FN_CALL 
 * 
 * - CODE_BLOCK -> BUILT_IN_VOID_FN_CALL 
 * 
 * This function uses following global variables:
 * 
 * - T_TOKEN token
 * 
 * - int error_flag
 * 
 * @return true if syntax is correct, false otherwise
 */
bool syntax_code_block() {
    // TODO: add semantic checks, cleaning, etc.

    // we have several branches, need to choose here
    next_token();
    token_buffer_move_back();
    switch (token.type) {
        case CONST:
        case VAR: 
            if (!syntax_var_def()) {
                return false;
            }
            break;
        case IF:
            if (!syntax_if_statement()) {
                return false;
            }
            break;
        case WHILE:
            if (!syntax_while_statement()) {
                return false;
            }
            break;
        case RETURN:
            if (!syntax_return()) {
                return false;
            }
            break;
        case IDENTIFIER:
            if (!syntax_assign_expr_or_fn_call()) {
                return false;
            }
            break;
        case IDENTIFIER_DISCARD:
            if (!syntax_assign_discard_expr_or_fn_call()) {
                return false;
            }
            break;
        case IFJ:
            if (!syntax_built_in_void_fn_call()) {
                return false;
            }
            break;
        default:
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
    }

    return true;
}

/**
 * @brief Checks syntax of variable definition. Simulates VAR_DEF non-terminal.
 * 
 * VAR_DEF is defined as:
 * VAR_DEF -> const identifier VAR_DEF_AFTER_ID
 * VAR_DEF -> var identifier VAR_DEF_AFTER_ID
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_var_def() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> const identifier VAR_DEF_AFTER_ID
    if (token.type == CONST) {
        // identifier
        next_token();
        if (token.type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal VAR_DEF_AFTER_ID
        if (!syntax_var_def_after_id()) {
            return false;
        }

        return true;
    }

    // second branch -> var identifier VAR_DEF_AFTER_ID
    if (token.type == VAR) {
        // identifier
        next_token();
        if (token.type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal VAR_DEF_AFTER_ID
        if (!syntax_var_def_after_id()) {
            return false;
        }

        return true;
    }

    // we should not reach this point
}

/**
 * @brief Checks syntax of the rest of var def. Simulates VAR_DEF_AFTER_ID non-terminal.
 * 
 * VAR_DEF_AFTER_ID is defined as:
 * VAR_DEF_AFTER_ID -> : TYPE = ASSIGN
 * VAR_DEF_AFTER_ID -> = ASSIGN
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_var_def_after_id() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches
    next_token();
    // first branch -> : TYPE = ASSIGN
    if (token.type == COLON) {
        // follows non-terminal TYPE
        if (!syntax_type()) {
            return false;
        }

        // =
        next_token();
        if (token.type != ASSIGN) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal ASSIGN
        if (!syntax_assign()) {
            return false;
        }
    
        return true;
    }

    // second branch -> = ASSIGN
    if (token.type == ASSIGN) {
        // follows non-terminal ASSIGN
        if (!syntax_assign()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of if. Simulates IF_STATEMENT non-terminal.
 * 
 * IF_STATEMENT is defined as:
 * IF_STATEMENT -> if ( EXPRESSION ) IF_STATEMENT_REMAINING
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_if_statement() {
    // TODO: add semantic checks, cleaning, etc.

    // if
    next_token();
    if (token.type != IF) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // (
    next_token();
    if (token.type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // TODO: change based on api of bottom-up parser
    // follows an EXPRESSION, switching to bottom-up parsing
    if (!syntax_expression()) {
        return false;
    }

    // )
    next_token();
    if (token.type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal IF_STATEMENT_REMAINING
    // not checking token here
    if (!syntax_if_statement_remaining()) {
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of rest of if. Simulates IF_STATEMENT_REMAINING non-terminal.
 * 
 * IF_STATEMENT_REMAINING is defined as:
 * IF_STATEMENT_REMAINING -> { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
 * IF_STATEMENT_REMAINING -> | identifier | { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_if_statement_remaining() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
    if (token.type == BRACKET_LEFT_CURLY) { // {
        // follows non-terminal CODE_BLOCK_NEXT, no need to choose a branch,
        // not checking token here
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // else
        next_token();
        if (token.type != ELSE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // {
        next_token();
        if (token.type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal CODE_BLOCK_NEXT
        // not checking token here
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> | identifier | { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
    if (token.type == PIPE) { // |
        // identifier
        next_token();
        if (token.type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // |
        next_token();
        if (token.type != PIPE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // {
        next_token();
        if (token.type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal CODE_BLOCK_NEXT
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // else
        next_token();
        if (token.type != ELSE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // {
        next_token();
        if (token.type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal CODE_BLOCK_NEXT
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of while. Simulates WHILE_STATEMENT non-terminal.
 * 
 * WHILE_STATEMENT is defined as:
 * WHILE_STATEMENT -> while ( EXPRESSION ) WHILE_STATEMENT_REMAINING
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_while_statement() {
    // TODO: add semantic checks, cleaning, etc.

    // while
    next_token();
    if (token.type != WHILE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // (
    next_token();
    if (token.type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // TODO: change based on api of bottom-up parser
    // follows an EXPRESSION, switching to bottom-up parsing
    if (!syntax_expression()) {
        return false;
    }

    // )
    next_token();
    if (token.type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal WHILE_STATEMENT_REMAINING
    if (!syntax_while_statement_remaining()) {
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of rest of while. Simulates WHILE_STATEMENT_REMAINING non-terminal.
 * 
 * WHILE_STATEMENT_REMAINING is defined as:
 * WHILE_STATEMENT_REMAINING -> { CODE_BLOCK_NEXT }
 * WHILE_STATEMENT_REMAINING -> | identifier | { CODE_BLOCK_NEXT }
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_while_statement_remaining() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> { CODE_BLOCK_NEXT }
    if (token.type == BRACKET_LEFT_CURLY) { // {
        // follows non-terminal CODE_BLOCK_NEXT
        // not checking token here
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> | identifier | { CODE_BLOCK_NEXT }
    if (token.type == PIPE) { // |
        // identifier
        next_token();
        if (token.type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // |
        next_token();
        if (token.type != PIPE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // {
        next_token();
        if (token.type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal CODE_BLOCK_NEXT
        // not checking token here
        if (!syntax_code_block_next()) {
            return false;
        }

        // }
        next_token();
        if (token.type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of return. Simulates RETURN non-terminal.
 * 
 * RETURN is defined as:
 * RETURN -> return RETURN_REMAINING
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_return() {
    // TODO: add semantic checks, cleaning, etc.

    // return
    next_token();
    if (token.type != RETURN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal RETURN_REMAINING
    if (!syntax_return_remaining()) {
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of return. Simulates RETURN_REMAINING non-terminal.
 * 
 * RETURN_REMAINING is defined as:
 * RETURN_REMAINING -> ;
 * RETURN_REMAINING -> ASSIGN
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_return_remaining() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> ;
    if (token.type == SEMICOLON) { // ;
        return true;
    }

    // second branch -> ASSIGN
    // TODO: is this correct? Need to verify
    if (token.type == IDENTIFIER || token.type == IFJ || is_token_in_expr()) {
        // follows non-terminal ASSIGN
        token_buffer_move_back();
        if (!syntax_assign()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of void built-in fn call. Simulates BUILT_IN_VOID_FN_CALL non-terminal.
 * 
 * BUILT_IN_VOID_FN_CALL is defined as:
 * BUILT_IN_VOID_FN_CALL -> ifj . identifier ( ARGUMENTS ) ;
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_built_in_void_fn_call() {
    // TODO: add semantic checks, cleaning, etc.

    // ifj
    next_token();
    if (token.type != IFJ) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // .
    next_token();
    if (token.type != DOT) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // identifier
    next_token();
    if (token.type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // (
    next_token();
    if (token.type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal ARGUMENTS
    if (!syntax_arguments()) {
        return false;
    }

    // )
    next_token();
    if (token.type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // ;
    next_token();
    if (token.type != SEMICOLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of assigning. Simulates ASSIGN_EXPR_OR_FN_CALL non-terminal.
 * 
 * ASSIGN_EXPR_OR_FN_CALL is defined as:
 * ASSIGN_EXPR_OR_FN_CALL -> identifier ID_START
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_assign_expr_or_fn_call() {
    // TODO: add semantic checks, cleaning, etc.
    
    // identifier
    next_token();
    if (token.type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal ID_START
    if (!syntax_id_start()) {
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of assigning. Simulates ASSIGN_DISCARD_EXPR_OR_FN_CALL non-terminal.
 * 
 * ASSIGN_DISCARD_EXPR_OR_FN_CALL is defined as:
 * ASSIGN_DISCARD_EXPR_OR_FN_CALL -> discard_identifier = ASSIGN
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_assign_discard_expr_or_fn_call() {
    // TODO: add semantic checks, cleaning, etc.

    // discard_identifier
    next_token();
    if (token.type != IDENTIFIER_DISCARD) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // =
    next_token();
    if (token.type != ASSIGN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal ASSIGN
    if (!syntax_assign()) {
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of subpart of assigning. Simulates ID_START non-terminal.
 * 
 * ID_START is defined as:
 * ID_START -> = ASSIGN
 * ID_START -> FUNCTION_ARGUMENTS
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_id_start() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> = ASSIGN
    if (token.type == ASSIGN) { // =
        // follows non-terminal ASSIGN
        if (!syntax_assign()) {
            return false;
        }

        return true;
    }

    // second branch -> FUNCTION_ARGUMENTS, return token
    if (token.type == BRACKET_LEFT_SIMPLE) { // (
        // follows non-terminal FUNCTION_ARGUMENTS
        token_buffer_move_back();
        if (!syntax_function_arguments()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of fn args. Simulates FUNCTION_ARGUMENTS non-terminal.
 * 
 * FUNCTION_ARGUMENTS is defined as:
 * FUNCTION_ARGUMENTS -> ( ARGUMENTS ) ;
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_function_arguments() {
    // TODO: add semantic checks, cleaning, etc.

    // (
    next_token();
    if (token.type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows non-terminal ARGUMENTS
    if (!syntax_arguments()) {
        return false;
    }

    // )
    next_token();
    if (token.type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // ;
    next_token();
    if (token.type != SEMICOLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of assign. Simulates ASSIGN non-terminal.
 * 
 * ASSIGN is defined as:
 * ASSIGN -> EXPRESSION ;
 * ASSIGN -> identifier ID_ASSIGN
 * ASSIGN -> ifj . identifier ( ARGUMENTS ) ;
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_assign() {
    // TODO: add semantic checks, cleaning, etc.

    // we have several branches, need to choose here
    next_token();
    // first branch -> ifj . identifier ( ARGUMENTS ) ;
    if (token.type == IFJ) {
        // .
        next_token();
        if (token.type != DOT) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // identifier
        next_token();
        if (token.type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // (
        next_token();
        if (token.type != BRACKET_LEFT_SIMPLE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // follows non-terminal ARGUMENTS
        if (!syntax_arguments()) {
            return false;
        }

        // )
        next_token();
        if (token.type != BRACKET_RIGHT_SIMPLE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // ;
        next_token();
        if (token.type != SEMICOLON) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> identifier ID_ASSIGN
    if (token.type == IDENTIFIER) { // identifier
        // follows non-terminal ID_ASSIGN
        if (!syntax_id_assign()) {
            return false;
        }

        return true;
    }

    // third branch -> EXPRESSION ;
    if (is_token_in_expr()) {
        // follows non-terminal EXPRESSION
        token_buffer_move_back();
        if (!syntax_expression()) {
            return false;
        }

        // ;
        next_token();
        if (token.type != SEMICOLON) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of assign. Simulates ID_ASSIGN non-terminal.
 * 
 * ID_ASSIGN is defined as:
 * ID_ASSIGN -> EXPRESSION ;
 * ID_ASSIGN -> FUNCTION_ARGUMENTS
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_id_assign() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> FUNCTION_ARGUMENTS
    if (token.type == BRACKET_LEFT_SIMPLE) { // (
        // follows non-terminal FUNCTION_ARGUMENTS
        token_buffer_move_back();
        if (!syntax_function_arguments()) {
            return false;
        }

        return true;
    }

    // second branch -> EXPRESSION ;
    if (is_token_in_expr()) {
        // follows non-terminal EXPRESSION
        // we have to move back twice to get to the beginning of the expression
        // we ate the first token of the expression as identifier in ASSIGN non-terminal
        token_buffer_move_back();
        token_buffer_move_back();
        if (!syntax_expression()) {
            return false;
        }

        // ;
        next_token();
        if (token.type != SEMICOLON) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of arguments. Simulates ARGUMENTS non-terminal.
 * 
 * ARGUMENTS is defined as:
 * ARGUMENTS -> ε
 * ARGUMENTS -> ARGUMENT ARGUMENT_NEXT
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_arguments() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    token_buffer_move_back();
    // first branch -> ε
    if (token.type == BRACKET_RIGHT_SIMPLE) { // )
        return true;
    }

    // second branch -> ARGUMENT ARGUMENT_NEXT
    if (token.type == IDENTIFIER || token.type == INT ||
        token.type == FLOAT || token.type == STRING || token.type == NULL) {
        if (!syntax_argument()) {
            return false;
        }
        if (!syntax_argument_next()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of following fn args. Simulates ARGUMENT_NEXT non-terminal.
 * 
 * ARGUMENT_NEXT is defined as:
 * ARGUMENT_NEXT -> ε
 * ARGUMENT_NEXT -> , ARGUMENT_AFTER_COMMA
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_argument_next() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    // first branch -> ε
    if (token.type == BRACKET_RIGHT_SIMPLE) { // )
        token_buffer_move_back();
        return true;
    }

    // second branch -> , ARGUMENT_AFTER_COMMA
    if (token.type == COMMA) { // ,
        if (!syntax_argument_after_comma()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of subpart of fn args. Simulates ARGUMENT_AFTER_COMMA non-terminal.
 * 
 * ARGUMENT_AFTER_COMMA is defined as:
 * ARGUMENT_AFTER_COMMA -> ε
 * ARGUMENT_AFTER_COMMA -> ARGUMENT ARGUMENT_NEXT
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_argument_after_comma() {
    // TODO: add semantic checks, cleaning, etc.

    // we have two possible branches, need to choose here
    next_token();
    token_buffer_move_back();
    // first branch -> ε
    if (token.type == BRACKET_RIGHT_SIMPLE) { // )
        return true;
    }

    // second branch -> ARGUMENT ARGUMENT_NEXT
    if (token.type == IDENTIFIER || token.type == INT ||
        token.type == FLOAT || token.type == STRING || token.type == NULL) {
        if (!syntax_argument()) {
            return false;
        }
        if (!syntax_argument_next()) {
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks syntax of fn arg. Simulates ARGUMENT non-terminal.
 * 
 * ARGUMENT is defined as:
 * ARGUMENT -> identifier
 * ARGUMENT -> int
 * ARGUMENT -> float
 * ARGUMENT -> string
 * ARGUMENT -> null
 * 
 * This function uses following global variables:
 * - T_TOKEN token
 * - int error_flag
 * @return true if syntax is correct, false otherwise
 */
bool syntax_argument() {
    // TODO: add semantic checks, cleaning, etc.

    // check first terminal in argument -> identifier, int, float, string, null
    next_token();
    if (token.type != IDENTIFIER && token.type != INT &&
        token.type != FLOAT && token.type != STRING && token.type != NULL) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}



