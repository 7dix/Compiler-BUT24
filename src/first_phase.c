// FILE: first_phase.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: First phase of the IFJ24 compiler for scanning and parsing function headers.
//        This phase quickly scans the input file to extract function signatures into symtable.
//        All tokens are stored in the token buffer for further processing during the main phase.
// TODO: add creation of function signatures into symtable
// TODO: improve comments

#include "first_phase.h"

int error_flag_fp = RET_VAL_OK;
bool needs_last_token = false;


bool get_save_token(T_TOKEN_BUFFER *token_buffer, T_TOKEN **token) {

    if (!needs_last_token)  {
        (*token) = (T_TOKEN *) malloc(sizeof(T_TOKEN));
        if ((*token) == NULL) {
            error_flag_fp = RET_VAL_INTERNAL_ERR;
            return false;
        }
        error_flag_fp = get_token(*token);
        if (error_flag_fp != 0) {
            return false;
        }

        if (!add_token_as_last(token_buffer, *token)) {
            error_flag_fp = RET_VAL_INTERNAL_ERR;
            return false;
        }
    }
    else {
        get_last_token(token_buffer, token);
        needs_last_token = false;
    }

    return true;
}

/**
 * @brief Runs first phase of the compiler
 * 
 * Scans the input file to extract function signatures. Saves them into the symtable.
 * All tokens are stored into the token buffer.
 * 
 * @param *token_buffer pointer to the token buffer
 * @return int
 * @retval RET_VAL_OK - success
 * @retval RET_VAL_LEXICAL_ERR - lexical error
 * @retval RET_VAL_SYNTAX_ERR - syntax error
 * @retval RET_VAL_INTERNAL_ERR - internal error
 */
int first_phase(T_TOKEN_BUFFER *token_buffer) {

    // Run simplified parser to obtain function signatures
    if (!syntax_fp_start(token_buffer)) {
        return error_flag_fp;
    }

    return RET_VAL_OK;
}

// now we will use a simplified ll-grammar to only check function headers
// function bodies are ignored, only curly brackets are checked so that
// we know where each function ends


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
bool syntax_fp_start(T_TOKEN_BUFFER *token_buffer) {
    // TODO: add semantic checks, cleaning, etc.

    if (!syntax_fp_prolog(token_buffer)) { // PROLOG
        return false;
    }
    if (!syntax_fp_fn_defs(token_buffer)) { // FN_DEFS
        return false;
    }
    if (!syntax_fp_end(token_buffer)) { // END
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_prolog(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;

    if (!get_save_token(buffer, &token)) // const
        return false;
    
    if (token->type != CONST) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // ifj
        return false;
    
    if (token->type != IFJ) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // =
        return false;

    if (token->type != ASSIGN) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    if (!get_save_token(buffer, &token)) // @import
        return false;

    if (token->type != IMPORT) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    if (!get_save_token(buffer, &token)) // (
        return false;

    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // string
        return false;

    if (token->type != STRING) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // check that string is equal to "ifj24.zig"
    if (strcmp(token->lexeme, "ifj24.zig") != 0) {
        // TODO: process error
        // TODO: is this syntax or semantic error?
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // )
        return false;

    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // ;
        return false;
    
    if (token->type != SEMICOLON) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_fn_defs(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.

    if (!syntax_fp_fn_def(buffer)) { // FN_DEF
        return false;
    }
    if (!syntax_fp_fn_def_next(buffer)) { // FN_DEF_NEXT
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_fn_def(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;

    if (!get_save_token(buffer, &token)) 
        return false; // pub
    if (token->type != PUB) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) 
        return false; // fn
    if (token->type != FN) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) 
        return false; // identifier
    if (token->type != IDENTIFIER) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) 
        return false; // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_fp_params(buffer)) { // PARAMS
        return false;
    }

    if (!get_save_token(buffer, &token)) 
        return false; // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_fp_fn_def_remaining(buffer)) { // FN_DEF_REMAINING
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_fn_def_next(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;
    
    // we have two branches, choose here
    if (!get_save_token(buffer, &token)) 
        return false;
    
    // first branch -> ε
    if (token->type == EOF_TOKEN) { // PREDICT
        return true;
    }

    // second branch -> FN_DEF FN_DEF_NEXT
    if (token->type == PUB) { // pub is first in FN_DEF
        
        needs_last_token = true; // token needed in FN_DEF

        if (!syntax_fp_fn_def(buffer)) { // FN_DEF
            return false;
        }
        if (!syntax_fp_fn_def_next(buffer)) { // FN_DEF_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag_fp = RET_VAL_SYNTAX_ERR;
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_fn_def_remaining(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    // TODO: possible simplification by checking only void type

    T_TOKEN *token;
    // we have two branches, choose here
    if (!get_save_token(buffer, &token)) 
        return false;
    // first branch -> TYPE { CODE_BLOCK_NEXT }
    // any that can be first in TYPE
    if (token->type == TYPE_INT || token->type == TYPE_FLOAT ||
        token->type == TYPE_STRING || token->type == TYPE_INT_NULL ||
        token->type == TYPE_FLOAT_NULL || token->type == TYPE_STRING_NULL) {

        needs_last_token = true; // token needed in TYPE
        if (!syntax_fp_type(buffer)) { // TYPE
            return false;
        }

        if (!get_save_token(buffer, &token)) 
        return false; // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag_fp = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!simulate_fn_body(buffer)) {
            return false;
        }

        return true;
    }

    // second branch -> void { CODE_BLOCK_NEXT }
    if (token->type == VOID) { // void

        if (!get_save_token(buffer, &token)) 
        return false; // {
        if (token->type != BRACKET_LEFT_CURLY) {
            // TODO: process error
            error_flag_fp = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!simulate_fn_body(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag_fp = RET_VAL_SYNTAX_ERR;
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_params(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;
    
    // we have two branches, choose here
    if (!get_save_token(buffer, &token)) 
        return false;
    needs_last_token = true; // token needed in both
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        return true;
    }
    // second branch -> PARAM PARAM_NEXT
    if (token->type == IDENTIFIER) { // identifier is first in PARAM

        if (!syntax_fp_param(buffer)) { // PARAM
            return false;
        }
        if (!syntax_fp_param_next(buffer)) { // PARAM_NEXT
            return false;
        }

        return true;
    }
    
    // TODO: process error
    error_flag_fp = RET_VAL_SYNTAX_ERR;
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_param(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;

    if (!get_save_token(buffer, &token)) 
        return false; // identifier
    if (token->type != IDENTIFIER) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) 
        return false; // :
    if (token->type != COLON) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_fp_type(buffer)) { // TYPE
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_type(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;

    // check whether token is one of the types
    if (!get_save_token(buffer, &token)) 
        return false;
    if (token->type != TYPE_INT && token->type != TYPE_FLOAT &&
        token->type != TYPE_STRING && token->type != TYPE_INT_NULL && 
        token->type != TYPE_FLOAT_NULL && token->type != TYPE_STRING_NULL) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_param_next(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;

    // we have two branches, choose here
    if (!get_save_token(buffer, &token)) 
        return false;
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        needs_last_token = true; // token needed
        return true;
    }

    // second branch -> , PARAM_AFTER_COMMA
    if (token->type == COMMA) { // ,

        if (!syntax_fp_param_after_comma(buffer)) { // PARAM_AFTER_COMMA
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag_fp = RET_VAL_SYNTAX_ERR;
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_param_after_comma(T_TOKEN_BUFFER *buffer) {
    // TODO: add semantic checks, cleaning, etc.
    T_TOKEN *token;

    // we have two branches, choose here
    if (!get_save_token(buffer, &token)) 
        return false;
    needs_last_token = true; // token needed in both branches
    // first branch -> ε
    if (token->type == BRACKET_RIGHT_SIMPLE) { // PREDICT
        return true;
    }

    // second branch -> PARAM PARAM_NEXT
    if (token->type == IDENTIFIER) { // identifier is first in PARAM
        
        if (!syntax_fp_param(buffer)) { // PARAM
            return false;
        }
        if (!syntax_fp_param_next(buffer)) { // PARAM_NEXT
            return false;
        }

        return true;
    }

    // TODO: process error
    error_flag_fp = RET_VAL_SYNTAX_ERR;
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
 * - `int error_flag_fp`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_end(T_TOKEN_BUFFER *buffer) {
    T_TOKEN *token;  
    // check last terminal in the program -> EOF
    if (!get_save_token(buffer, &token)) 
        return false;
    if (token->type != EOF_TOKEN) {
        // TODO: process error
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

bool simulate_fn_body(T_TOKEN_BUFFER *buffer) {
    int bracket_count = 1;
    T_TOKEN *token;

    while (true) {
        if (!get_save_token(buffer, &token)) {
            return false;
        }
        
        switch (token->type) {
            case BRACKET_LEFT_CURLY:
                bracket_count++;
                break;

            case BRACKET_RIGHT_CURLY:
                bracket_count--;
                break;

            case EOF_TOKEN:
                return false;
            default:
                break;
        }

        if (bracket_count == 0) {
            return true;
        }
    }
}