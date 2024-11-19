// FILE: parser.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: Syntax-driven compilation for the IFJ24 language.
// TODO: maybe remove matching of prolog and other such stuff, as this is already checked in the first phase

#include "parser.h"

// Global variables

// Stores error defined in  `shared.h`
int error_flag = RET_VAL_OK;


/**
 * @brief Entry point of the parser.
 * 
 * Runs full syntax-driven compilation. Uses supplied token buffer.
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @note TODO: add semantic checks, cleaning, etc.
 * @note TODO: change signature to work properly with future symtable
 * @note TODO: improve comments on return values
 * @param *token_buffer pointer to token buffer
 * @return `int`
 * @retval `RET_VAL_OK` if parsing was successful
 * @retval `RET_VAL_SYNTAX_ERR` if syntax error occurred
 * @retval `RET_VAL_INTERNAL_ERR` if internal error occurred
 */
int run_parser(T_TOKEN_BUFFER *token_buffer) {
    // TODO: add semantic checks, cleaning, etc.
    // Start of recursive parser
    if (!syntax_start(token_buffer)) {
        return error_flag;
    }
    return RET_VAL_OK;
}

/**
 * @brief Checks whether given token can occur in expression.
 * 
 * @param *token pointer to token
 * @return `bool`
 * @retval `true` - if token can occur in expression
 * @retval `false` - otherwise
 */
bool is_token_in_expr(T_TOKEN *token) {
    return (    token->type == NOT_EQUAL || token->type == INT ||
                token->type == FLOAT || token->type == LESS_THAN ||
                token->type == GREATER_THAN || token->type == EQUAL ||
                token->type == LESS_THAN_EQUAL || token->type == GREATER_THAN_EQUAL ||
                token->type == PLUS || token->type == MINUS || token->type == MULTIPLY ||
                token->type == DIVIDE || token->type == BRACKET_LEFT_SIMPLE ||
                token->type == BRACKET_RIGHT_SIMPLE || token->type == IDENTIFIER );
}

/**
 * @brief Start of recursive parser. Simulates `START` non-terminal.
 * 
 * `START` is defined as:
 * 
 * `START -> PROLOG FN_DEFS END`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_start(T_TOKEN_BUFFER *token_buffer) {
    // TODO: add semantic checks, cleaning, etc.

    if (!syntax_prolog(token_buffer)) { // PROLOG
        return false;
    }
    if (!syntax_fn_defs(token_buffer)) { // FN_DEFS
        return false;
    }
    if (!syntax_end(token_buffer)) { // END
        return false;
    }
    return true;
}

/**
 * @brief Simulates `PROLOG` non-terminal.
 * 
 * `PROLOG` is defined as:
 * 
 * `PROLOG -> const ifj = @import ( "ifj24.zig" ) ;`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_prolog(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;

    next_token(buffer, &token); // const
    if (token->type != CONST) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ifj
    if (token->type != IFJ) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // =
    if (token->type != ASSIGN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    next_token(buffer, &token); // @import
    if (token->type != IMPORT) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // string
    if (token->type != STRING) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // check that string is equal to "ifj24.zig"
    if (strcmp(token->value.stringVal, "ifj24.zig") != 0) {
        // TODO: process error
        // TODO: is this syntax or semantic error?
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ;
    if (token->type != SEMICOLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Simulates `FN_DEFS` non-terminal.
 * 
 * `FN_DEFS` is defined as:
 * 
 * `FN_DEFS -> FN_DEF FN_DEF_NEXT`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fn_defs(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    if (!syntax_fn_def(buffer)) { // FN_DEF
        return false;
    }
    if (!syntax_fn_def_next(buffer)) { // FN_DEF_NEXT
        return false;
    }
    return true;
}

/**
 * @brief Simulates `FN_DEF` non-terminal.
 * 
 * `FN_DEF` is defined as:
 * 
 * `FN_DEF -> pub fn identifier ( PARAMS ) FN_DEF_REMAINING`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fn_def(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;

    next_token(buffer, &token); // pub
    if (token->type != PUB) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // fn
    if (token->type != FN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_params(buffer)) { // PARAMS
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_fn_def_remaining(buffer)) { // FN_DEF_REMAINING
        return false;
    }

    return true;
}

/**
 * @brief Simulates `FN_DEF_NEXT` non-terminal.
 * 
 * `FN_DEF_NEXT` is defined as:
 * 
 * `FN_DEF_NEXT -> ε`
 * 
 * `FN_DEF_NEXT -> FN_DEF FN_DEF_NEXT`
 * 
 * This function uses following global variables:
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fn_def_next(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    
    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    
    // first branch -> ε
    if (token->type == EOF_TOKEN) { // PREDICT
        return true;
    }

    // second branch -> FN_DEF FN_DEF_NEXT
    if (token->type == PUB) { // pub is first in FN_DEF
        
        move_back(buffer); // token needed in FN_DEF

        if (!syntax_fn_def(buffer)) { // FN_DEF
            return false;
        }
        if (!syntax_fn_def_next(buffer)) { // FN_DEF_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `FN_DEF_REMAINING` non-terminal.
 * 
 * `FN_DEF_REMAINING` is defined as:
 * 
 * `FN_DEF_REMAINING -> TYPE { CODE_BLOCK_NEXT }`
 * 
 * `FN_DEF_REMAINING -> void { CODE_BLOCK_NEXT }`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fn_def_remaining(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    // TODO: possible simplification by checking only void type
    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> TYPE { CODE_BLOCK_NEXT }
    // any that can be first in TYPE
    if (token->type == TYPE_INT || token->type == TYPE_FLOAT ||
        token->type == TYPE_STRING || token->type == TYPE_INT_NULL ||
        token->type == TYPE_FLOAT_NULL || token->type == TYPE_STRING_NULL) {

        move_back(buffer); // token needed in TYPE
        if (!syntax_type(buffer)) { // TYPE
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> void { CODE_BLOCK_NEXT }
    if (token->type == VOID) { // void

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
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
 * @brief Simulates `PARAMS` non-terminal.
 * 
 * `PARAMS` is defined as:
 * 
 * `PARAMS -> ε`
 * 
 * `PARAMS -> PARAM PARAM_NEXT`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_params(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    move_back(buffer); // token needed in both
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        return true;
    }
    // second branch -> PARAM PARAM_NEXT
    if (token->type == IDENTIFIER) { // identifier is first in PARAM

        if (!syntax_param(buffer)) { // PARAM
            return false;
        }
        if (!syntax_param_next(buffer)) { // PARAM_NEXT
            return false;
        }

        return true;
    }
    
    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `PARAM` non-terminal.
 * 
 * `PARAM` is defined as:
 * 
 * `PARAM -> identifier : TYPE`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_param(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // :
    if (token->type != COLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_type(buffer)) { // TYPE
        return false;
    }

    return true;
}

/**
 * @brief Simulates `TYPE` non-terminal.
 * 
 * `TYPE` is defined as:
 * 
 * `TYPE -> type_int`
 * 
 * `TYPE -> type_float`
 * 
 * `TYPE -> type_string`
 * 
 * `TYPE -> type_int_null`
 * 
 * `TYPE -> type_float_null`
 * 
 * `TYPE -> type_string_null`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_type(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // check whether token is one of the types
    next_token(buffer, &token);
    if (token->type != TYPE_INT && token->type != TYPE_FLOAT &&
        token->type != TYPE_STRING && token->type != TYPE_INT_NULL && 
        token->type != TYPE_FLOAT_NULL && token->type != TYPE_STRING_NULL) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Simulates `PARAM_NEXT` non-terminal.
 * 
 * `PARAM_NEXT` is defined as:
 * 
 * `PARAM_NEXT -> ε`
 * 
 * `PARAM_NEXT -> , PARAM_AFTER_COMMA`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_param_next(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        move_back(buffer); // token needed
        return true;
    }

    // second branch -> , PARAM_AFTER_COMMA
    if (token->type == COMMA) { // ,

        if (!syntax_param_after_comma(buffer)) { // PARAM_AFTER_COMMA
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `PARAM_AFTER_COMMA` non-terminal.
 * 
 * `PARAM_AFTER_COMMA is defined as:
 * 
 * `PARAM_AFTER_COMMA -> ε`
 * 
 * `PARAM_AFTER_COMMA -> PARAM PARAM_NEXT`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_param_after_comma(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    move_back(buffer); // token needed in both branches
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        return true;
    }

    // second branch -> PARAM PARAM_NEXT
    if (token->type == IDENTIFIER) { // identifier is first in PARAM
        
        if (!syntax_param(buffer)) { // PARAM
            return false;
        }
        if (!syntax_param_next(buffer)) { // PARAM_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Checks end of file. Simulates `END` non-terminal.
 * 
 * `END` is defined as:
 * 
 * `END -> eof_token`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_end(T_TOKEN_BUFFER *buffer) {
    T_TOKEN *token;
    // check last terminal in the program -> EOF
    next_token(buffer, &token);
    if (token->type != EOF_TOKEN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Simulates `CODE_BLOCK_NEXT` non-terminal.
 * 
 * `CODE_BLOCK_NEXT` is defined as:
 * 
 * `CODE_BLOCK_NEXT -> ε`
 * 
 * `CODE_BLOCK_NEXT -> CODE_BLOCK CODE_BLOCK_NEXT`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_code_block_next(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    move_back(buffer); // token needed in both branches
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_CURLY) { // PREDICT
        return true;
    }

    // second branch -> CODE_BLOCK CODE_BLOCK_NEXT
    // is first in CODE_BLOCK ?
    if (token->type == CONST || token->type == VAR || token->type == IF ||
        token->type == WHILE || token->type == RETURN || token->type == IFJ ||
        token->type == IDENTIFIER || token->type == IDENTIFIER_DISCARD) {
        
        if (!syntax_code_block(buffer)) { // CODE_BLOCK
            return false;
        }
        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `CODE_BLOCK` non-terminal.
 * 
 * `CODE_BLOCK` is defined as:
 * 
 * `CODE_BLOCK -> VAR_DEF`
 * 
 * `CODE_BLOCK -> IF_STATEMENT`
 * 
 * `CODE_BLOCK -> WHILE_STATEMENT`
 * 
 * `CODE_BLOCK -> RETURN`
 * 
 * `CODE_BLOCK -> ASSIGN_EXPR_OR_FN_CALL`
 * 
 * `CODE_BLOCK -> ASSIGN_DISCARD_EXPR_OR_FN_CALL` 
 * 
 * `CODE_BLOCK -> BUILT_IN_VOID_FN_CALL` 
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_code_block(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have several branches, choose here
    next_token(buffer, &token);
    move_back(buffer);
    switch (token->type) {
        case CONST:
        case VAR: 
            if (!syntax_var_def(buffer)) {
                return false;
            }
            break;
        case IF:
            if (!syntax_if_statement(buffer)) {
                return false;
            }
            break;
        case WHILE:
            if (!syntax_while_statement(buffer)) {
                return false;
            }
            break;
        case RETURN:
            if (!syntax_return(buffer)) {
                return false;
            }
            break;
        case IDENTIFIER:
            if (!syntax_assign_expr_or_fn_call(buffer)) {
                return false;
            }
            break;
        case IDENTIFIER_DISCARD:
            if (!syntax_assign_discard_expr_or_fn_call(buffer)) {
                return false;
            }
            break;
        case IFJ:
            if (!syntax_built_in_void_fn_call(buffer)) {
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
 * @brief Simulates `VAR_DEF` non-terminal.
 * 
 * `VAR_DEF` is defined as:
 * `VAR_DEF -> const identifier VAR_DEF_AFTER_ID`
 * `VAR_DEF -> var identifier VAR_DEF_AFTER_ID`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_var_def(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> const identifier VAR_DEF_AFTER_ID
    if (token->type == CONST) { // const

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_var_def_after_id(buffer)) { // VAR_DEF_AFTER_ID
            return false;
        }

        return true;
    }

    // second branch -> var identifier VAR_DEF_AFTER_ID
    if (token->type == VAR) { // var
        
        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_var_def_after_id(buffer)) { // VAR_DEF_AFTER_ID
            return false;
        }

        return true;
    }

    // we should not reach this point, anyway ...
    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `VAR_DEF_AFTER_ID` non-terminal.
 * 
 * `VAR_DEF_AFTER_ID` is defined as:
 * 
 * `VAR_DEF_AFTER_ID -> : TYPE = ASSIGN`
 * 
 * `VAR_DEF_AFTER_ID -> = ASSIGN`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_var_def_after_id(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> : TYPE = ASSIGN
    if (token->type == COLON) { // :

        if (!syntax_type(buffer)) { // TYPE
            return false;
        }

        next_token(buffer, &token);
        if (token->type != ASSIGN) { // =
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_assign(buffer)) { // ASSIGN
            return false;
        }
    
        return true;
    }

    // second branch -> = ASSIGN
    if (token->type == ASSIGN) { // =

        if (!syntax_assign(buffer)) { // ASSIGN
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `IF_STATEMENT` non-terminal.
 * 
 * `IF_STATEMENT` is defined as:
 * 
 * `IF_STATEMENT -> if ( EXPRESSION ) IF_STATEMENT_REMAINING`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_if_statement(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;

    next_token(buffer, &token); // if
    if (token->type != IF) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // follows an EXPRESSION, switching to bottom-up parsing
    T_TREE_NODE_PTR tree;
    tree_init(&tree);
    error_flag = precedenceSyntaxMain(buffer, &tree, IF_WHILE_END);
    if (error_flag != RET_VAL_OK) {
        return false;
    }
    tree_dispose(&tree);

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_if_statement_remaining(buffer)) { // IF_STATEMENT_REMAINING
        return false;
    }

    return true;
}

/**
 * @brief Simulates `IF_STATEMENT_REMAINING` non-terminal.
 * 
 * `IF_STATEMENT_REMAINING` is defined as:
 * 
 * `IF_STATEMENT_REMAINING -> { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }`
 * 
 * `IF_STATEMENT_REMAINING -> | identifier | { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_if_statement_remaining(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
    if (token->type == BRACKET_LEFT_CURLY) { // {
        
        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // else
        if (token->type != ELSE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> | identifier | { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
    if (token->type == PIPE) { // |

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // |
        if (token->type != PIPE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // else
        if (token->type != ELSE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
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
 * @brief Simulates `WHILE_STATEMENT` non-terminal.
 * 
 * `WHILE_STATEMENT` is defined as:
 * 
 * `WHILE_STATEMENT -> while ( EXPRESSION ) WHILE_STATEMENT_REMAINING`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_while_statement(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;

    next_token(buffer, &token); // while
    if (token->type != WHILE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // TODO: change based on api of bottom-up parser
    // follows an EXPRESSION, switching to bottom-up parsing
    T_TREE_NODE_PTR tree;
    tree_init(&tree);
    error_flag = precedenceSyntaxMain(buffer, &tree, IF_WHILE_END);
    if (error_flag != RET_VAL_OK) {
        return false;
    }
    tree_dispose(&tree);

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_while_statement_remaining(buffer)) { // WHILE_STATEMENT_REMAINING
        return false;
    }

    return true;
}

/**
 * @brief Simulates `WHILE_STATEMENT_REMAINING` non-terminal.
 * 
 * `WHILE_STATEMENT_REMAINING` is defined as:
 * `WHILE_STATEMENT_REMAINING -> { CODE_BLOCK_NEXT }`
 * `WHILE_STATEMENT_REMAINING -> | identifier | { CODE_BLOCK_NEXT }`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_while_statement_remaining(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> { CODE_BLOCK_NEXT }
    if (token->type == BRACKET_LEFT_CURLY) { // {

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> | identifier | { CODE_BLOCK_NEXT }
    if (token->type == PIPE) { // |

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // |
        if (token->type != PIPE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
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
 * @brief Simulates `RETURN` non-terminal.
 * 
 * `RETURN` is defined as:
 * 
 * `RETURN -> return RETURN_REMAINING`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_return(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;

    next_token(buffer, &token); // return
    if (token->type != RETURN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_return_remaining(buffer)) { // RETURN_REMAINING
        return false;
    }

    return true;
}

/**
 * @brief Simulates `RETURN_REMAINING` non-terminal.
 * 
 * `RETURN_REMAINING` is defined as:
 * 
 * `RETURN_REMAINING -> ;`
 * 
 * `RETURN_REMAINING -> ASSIGN`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_return_remaining(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> ;
    if (token->type == SEMICOLON) { // ;
        return true;
    }

    // second branch -> ASSIGN, check first token in ASSIGN
    if (token->type == IDENTIFIER || token->type == IFJ || is_token_in_expr(token)) {
        move_back(buffer); // token needed in ASSIGN
        if (!syntax_assign(buffer)) { // ASSIGN
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `BUILT_IN_VOID_FN_CALL` non-terminal.
 * 
 * `BUILT_IN_VOID_FN_CALL` is defined as:
 * 
 * `BUILT_IN_VOID_FN_CALL -> ifj . identifier ( ARGUMENTS ) ;`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_built_in_void_fn_call(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;

    next_token(buffer, &token); // ifj
    if (token->type != IFJ) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // .
    if (token->type != DOT) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_arguments(buffer)) { // ARGUMENTS
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ;
    if (token->type != SEMICOLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Simulates `ASSIGN_EXPR_OR_FN_CALL` non-terminal.
 * 
 * `ASSIGN_EXPR_OR_FN_CALL` is defined as:
 * 
 * `ASSIGN_EXPR_OR_FN_CALL -> identifier ID_START`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_assign_expr_or_fn_call(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    
    T_TOKEN *token;

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_id_start(buffer)) { // ID_START
        return false;
    }

    return true;
}

/**
 * @brief Simulates `ASSIGN_DISCARD_EXPR_OR_FN_CALL` non-terminal.
 * 
 * `ASSIGN_DISCARD_EXPR_OR_FN_CALL` is defined as:
 * 
 * `ASSIGN_DISCARD_EXPR_OR_FN_CALL -> discard_identifier = ASSIGN`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_assign_discard_expr_or_fn_call(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;

    next_token(buffer, &token); // discard_identifier
    if (token->type != IDENTIFIER_DISCARD) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // =
    if (token->type != ASSIGN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_assign(buffer)) { // ASSIGN
        return false;
    }

    return true;
}

/**
 * @brief Simulates `ID_START` non-terminal.
 * 
 * `ID_START` is defined as:
 * 
 * `ID_START -> = ASSIGN`
 * 
 * `ID_START -> FUNCTION_ARGUMENTS`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_id_start(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> = ASSIGN
    if (token->type == ASSIGN) { // =
        
        if (!syntax_assign(buffer)) { // ASSIGN
            return false;
        }

        return true;
    }

    // second branch -> FUNCTION_ARGUMENTS
    if (token->type == BRACKET_LEFT_SIMPLE) { // (
        
        move_back(buffer); // token needed in FUNCTION_ARGUMENTS
        if (!syntax_function_arguments(buffer)) { // FUNCTION_ARGUMENTS
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `FUNCTION_ARGUMENTS` non-terminal.
 * 
 * `FUNCTION_ARGUMENTS` is defined as:
 * 
 * `FUNCTION_ARGUMENTS -> ( ARGUMENTS ) ;`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_function_arguments(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    
    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_arguments(buffer)) { // ARGUMENTS
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ;
    if (token->type != SEMICOLON) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Simulates `ASSIGN` non-terminal.
 * 
 * `ASSIGN` is defined as:
 * 
 * `ASSIGN -> null ;`
 * 
 * `ASSIGN -> EXPRESSION ;`
 * 
 * `ASSIGN -> identifier ID_ASSIGN`
 * 
 * `ASSIGN -> ifj . identifier ( ARGUMENTS ) ;`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_assign(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have several branches, choose here
    next_token(buffer, &token);
    // first branch -> null ;
    if (token->type == NULL_TOKEN) { // null

        next_token(buffer, &token); // ;
        if (token->type != SEMICOLON) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }
    
    // second branch -> ifj . identifier ( ARGUMENTS ) ;
    if (token->type == IFJ) { // ifj

        next_token(buffer, &token); // .
        if (token->type != DOT) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // (
        if (token->type != BRACKET_LEFT_SIMPLE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_arguments(buffer)) { // ARGUMENTS
            return false;
        }

        next_token(buffer, &token); // )
        if (token->type != BRACKET_RIGHT_SIMPLE) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // ;
        if (token->type != SEMICOLON) {
            // TODO: process error
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // third branch -> identifier ID_ASSIGN
    if (token->type == IDENTIFIER) { // identifier

        if (!syntax_id_assign(buffer)) { // ID_ASSIGN
            return false;
        }

        return true;
    }

    // fourth branch -> EXPRESSION ;
    if (is_token_in_expr(token)) {
        move_back(buffer);
        // TODO: change based on api of bottom-up parser
        T_TREE_NODE_PTR tree;
        tree_init(&tree);
        error_flag = precedenceSyntaxMain(buffer, &tree, ASS_END);
        if (error_flag != RET_VAL_OK) {
            return false;
        }
        tree_dispose(&tree);

        next_token(buffer, &token); // ;
        if (token->type != SEMICOLON) {
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
 * @brief Simulates `ID_ASSIGN` non-terminal.
 * 
 * `ID_ASSIGN` is defined as:
 * 
 * `ID_ASSIGN -> EXPRESSION ;`
 * 
 * `ID_ASSIGN -> FUNCTION_ARGUMENTS`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_id_assign(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> FUNCTION_ARGUMENTS
    if (token->type == BRACKET_LEFT_SIMPLE) { // (
        
        move_back(buffer); // token needed in FUNCTION_ARGUMENTS
        if (!syntax_function_arguments(buffer)) { // FUNCTION_ARGUMENTS
            return false;
        }

        return true;
    }

    // second branch -> EXPRESSION ;
    if (is_token_in_expr(token)) { // is token in expression ?
        // we have to move back twice to get to the beginning of the expression
        // we ate the first token of the expression as identifier in ASSIGN non-terminal
        move_back(buffer);
        move_back(buffer);
        T_TREE_NODE_PTR tree;
        tree_init(&tree);
        error_flag = precedenceSyntaxMain(buffer, &tree, IF_WHILE_END);
        if (error_flag != RET_VAL_OK) {
            return false;
        }
        tree_dispose(&tree);

        next_token(buffer, &token); // ;
        if (token->type != SEMICOLON) {
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
 * @brief Simulates `ARGUMENTS` non-terminal.
 * 
 * `ARGUMENTS` is defined as:
 * 
 * `ARGUMENTS -> ε`
 * 
 * `ARGUMENTS -> ARGUMENT ARGUMENT_NEXT`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_arguments(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    move_back(buffer);
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        return true;
    }

    // second branch -> ARGUMENT ARGUMENT_NEXT, is it first in ARGUMENT ?
    if (token->type == IDENTIFIER || token->type == INT ||
        token->type == FLOAT || token->type == STRING || token->type == NULL_TOKEN) {

        if (!syntax_argument(buffer)) { // ARGUMENT
            return false;
        }
        if (!syntax_argument_next(buffer)) { // ARGUMENT_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `ARGUMENT_NEXT` non-terminal.
 * 
 * `ARGUMENT_NEXT` is defined as:
 * 
 * `ARGUMENT_NEXT -> ε`
 * 
 * `ARGUMENT_NEXT -> , ARGUMENT_AFTER_COMMA`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_argument_next(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        move_back(buffer);
        return true;
    }

    // second branch -> , ARGUMENT_AFTER_COMMA
    if (token->type == COMMA) { // ,

        if (!syntax_argument_after_comma(buffer)) { // ARGUMENT_AFTER_COMMA
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `ARGUMENT_AFTER_COMMA` non-terminal.
 * 
 * `ARGUMENT_AFTER_COMMA` is defined as:
 * 
 * `ARGUMENT_AFTER_COMMA -> ε`
 * 
 * `ARGUMENT_AFTER_COMMA -> ARGUMENT ARGUMENT_NEXT`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_argument_after_comma(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    move_back(buffer);
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        return true;
    }

    // second branch -> ARGUMENT ARGUMENT_NEXT, is it first in ARGUMENT ?
    if (token->type == IDENTIFIER || token->type == INT ||
        token->type == FLOAT || token->type == STRING || token->type == NULL_TOKEN) {

        if (!syntax_argument(buffer)) { // ARGUMENT
            return false;
        }
        if (!syntax_argument_next(buffer)) { // ARGUMENT_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag = RET_VAL_SYNTAX_ERR;
    return false;
}

/**
 * @brief Simulates `ARGUMENT` non-terminal.
 * 
 * `ARGUMENT` is defined as:
 * 
 * `ARGUMENT -> identifier`
 * 
 * `ARGUMENT -> int`
 * 
 * `ARGUMENT -> float`
 * 
 * `ARGUMENT -> string`
 * 
 * `ARGUMENT -> null`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_argument(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // check first terminal in argument -> identifier, int, float, string, null
    next_token(buffer, &token);
    if (token->type != IDENTIFIER && token->type != INT &&
        token->type != FLOAT && token->type != STRING && token->type != NULL_TOKEN) {
        // TODO: process error
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}
