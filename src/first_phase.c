// FILE: first_phase.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí>   (xkocio00)
//  <Marek Tenora>  (xtenor02)
//
// YEAR: 2024
// NOTES: First phase of the IFJ24 compiler for scanning and parsing function headers.
//        This phase quickly scans the input file to extract function signatures into symtable.
//        All tokens are stored in the token buffer for further processing during the main phase.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "token_buffer.h"
#include "symtable.h"
#include "scanner.h"
#include "return_values.h"
#include "first_phase.h"


//--------------------------- GLOBAL VARIABLES ----------------------------//

// Stores any error generated during this phase, see `return_values.h`
RET_VAL error_flag_fp = RET_VAL_OK;

// Flag indicating whether get_save_token() should return the last token
// that is already in the buffer, or ask lexer for a new one and return that one 
bool needs_last_token = false;


//------------------ PRIVATE FUNCTION PROTOTYPES --------------------------//

bool get_save_token(T_TOKEN_BUFFER *token_buffer, T_TOKEN **token);
bool syntax_fp_start(T_TOKEN_BUFFER *buffer);
bool syntax_fp_prolog(T_TOKEN_BUFFER *buffer);
bool syntax_fp_fn_def(T_TOKEN_BUFFER *buffer);
bool syntax_fp_fn_def_next(T_TOKEN_BUFFER *buffer);
bool syntax_fp_fn_def_remaining(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data);
bool syntax_fp_type(T_TOKEN_BUFFER *buffer, VAR_TYPE *type);
bool syntax_fp_params(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data);
bool syntax_fp_param(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data);
bool syntax_fp_param_next(T_TOKEN_BUFFER *buffe, T_SYMBOL_DATA *data);
bool syntax_fp_param_after_comma(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data);
bool syntax_fp_end(T_TOKEN_BUFFER *buffer);
bool simulate_fn_body(T_TOKEN_BUFFER *buffer);


/**
 * @brief Fills symtable with built-in functions
 * 
 * @return `bool`
 * @retval `true` - success
 * @retval `false` - internal error occurred
 */
bool add_built_in_functions() {
    T_SYMBOL_DATA data;
    T_SYMBOL *symbol;
    
    // ifj.readstr() ?[]u8
    data.func.return_type = VAR_STRING_NULL;
    data.func.argc = 0;
    data.func.argv = NULL;
    if ((symbol = symtable_add_symbol(ST, "ifj.readstr", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }

    // ifj.readi32() ?i32
    data.func.return_type = VAR_INT_NULL;
    if ((symbol = symtable_add_symbol(ST, "ifj.readi32", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }

    // ifj.readf64() ?f64
    data.func.return_type = VAR_FLOAT_NULL;
    if ((symbol = symtable_add_symbol(ST, "ifj.readf64", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }

    // ifj.write(term: any) void
    data.func.return_type = VAR_VOID;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"term", VAR_ANY});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.write", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.i2f(term: i32) f64
    data.func.return_type = VAR_FLOAT;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"term", VAR_INT});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.i2f", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.f2i(term: f64) i32
    data.func.return_type = VAR_INT;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"term", VAR_FLOAT});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.f2i", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.string(term: str_u8) []u8
    data.func.return_type = VAR_STRING;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"term", STRING_VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.string", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.length(s: []u8) i32
    data.func.return_type = VAR_INT;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"s", VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.length", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.concat(s1: []u8, s2: []u8) []u8
    data.func.return_type = VAR_STRING;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"s1", VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"s2", VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.concat", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.substr(s: []u8, i: i32, j: i32) ?[]u8
    data.func.return_type = VAR_STRING_NULL;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"s", VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"i", VAR_INT});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"j", VAR_INT});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.substring", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.strcmp(s1: []u8, s2: []u8) i32
    data.func.return_type = VAR_INT;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"s1", VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"s2", VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.strcmp", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;

    // ifj.ord(s: []u8, i: i32) i32
    data.func.return_type = VAR_INT;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"s", VAR_STRING});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"i", VAR_INT});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.ord", SYM_FUNC, data)) == NULL) {
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }
    data.func.argc = 0;
    data.func.argv = NULL;
    
    // ifj.chr(i: i32) []u8
    data.func.return_type = VAR_STRING;
    error_flag_fp = add_param_to_symbol_data(&data, (T_PARAM){"i", VAR_INT});
    if (error_flag_fp != RET_VAL_OK)
        return false;
    if ((symbol = symtable_add_symbol(ST, "ifj.chr", SYM_FUNC, data)) == NULL){
        error_flag_fp = RET_VAL_INTERNAL_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Checks if main function exists and is correctly defined
 * 
 * @return `bool`
 * @retval `true` - main function exists
 * @retval `false` - main function does not exist
 */
bool check_main_exists() {
    T_SYMBOL *symbol;
    // exists main
    if ((symbol = symtable_find_symbol(ST, "main")) == NULL) {
        error_flag_fp = RET_VAL_SEMANTIC_UNDEFINED_ERR;
        return false;
    }
    // has it void return type
    if (symbol->data.func.return_type != VAR_VOID) {
        error_flag_fp = RET_VAL_SEMANTIC_FUNCTION_ERR;
        return false;
    }
    // it should have no arguments
    if (symbol->data.func.argc != 0) {
        error_flag_fp = RET_VAL_SEMANTIC_FUNCTION_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Gets and saves token into the token buffer
 * 
 * @param *token_buffer pointer to the token buffer
 * @param **token pointer to the token
 * 
 * Asks the lexer for a new token, that is saved into token buffer
 * and returned to the caller. If needs_last_token flag is set, last token
 * from the buffer is returned instead.
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag_fp`
 * - `bool needs_last_token`
 * 
 * @return `bool`
 * @retval `true` - success
 * @retval `false` - internal error occurred
 */
bool get_save_token(T_TOKEN_BUFFER *token_buffer, T_TOKEN **token) {

    if (!needs_last_token)  { // get new token
        (*token) = (T_TOKEN *) malloc(sizeof(T_TOKEN));
        if ((*token) == NULL) {
            error_flag_fp = RET_VAL_INTERNAL_ERR;
            return false;
        }
        (*token)->lexeme = NULL;
        (*token)->value.str_val = NULL;

        error_flag_fp = get_token(*token);
        if (error_flag_fp != RET_VAL_OK) {
            free((*token));
            return false;
        }

        // save to buffer
        if (!add_token_as_last(token_buffer, *token)) {
            error_flag_fp = RET_VAL_INTERNAL_ERR;
            free((*token));
            return false;
        }
    }
    else { // last token is requested
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
 * This function uses following global variables:
 * 
 * - `int error_flag_fp`
 * 
 * @param *token_buffer pointer to the token buffer
 * @return int
 * @retval RET_VAL_OK - success
 * @retval RET_VAL_LEXICAL_ERR - lexical error
 * @retval RET_VAL_SYNTAX_ERR - syntax error
 * @retval RET_VAL_INTERNAL_ERR - internal error
 */
RET_VAL first_phase(T_TOKEN_BUFFER *token_buffer) {

    // Run simplified parser to obtain function signatures
    if (!syntax_fp_start(token_buffer)) {
        return error_flag_fp;
    }

    // Check main existence
    if (!check_main_exists()) {
        return error_flag_fp;
    }

    // Add built-in functions to symtable
    if (!add_built_in_functions()) {
        return error_flag_fp;
    }

    return RET_VAL_OK;
}

/**
 * @brief Start of recursive parser. Simulates `START` non-terminal.
 * 
 * `START` is defined as:
 * 
 * `START -> PROLOG FN_DEF_NEXT END`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_start(T_TOKEN_BUFFER *token_buffer) {

    if (!syntax_fp_prolog(token_buffer)) { // PROLOG
        return false;
    }
    if (!syntax_fp_fn_def_next(token_buffer)) { // FN_DEF_NEXT
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
    
    T_TOKEN *token;
    if (!get_save_token(buffer, &token)) // const
        return false;
    
    if (token->type != CONST) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // ifj
        return false;
    
    if (token->type != IFJ) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // =
        return false;

    if (token->type != ASSIGN) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    if (!get_save_token(buffer, &token)) // @import
        return false;

    if (token->type != IMPORT) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    if (!get_save_token(buffer, &token)) // (
        return false;

    if (token->type != BRACKET_LEFT_SIMPLE) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // string
        return false;

    if (token->type != STRING) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // check that string is equal to "ifj24.zig"
    if (strcmp(token->value.str_val, "ifj24.zig") != 0) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // )
        return false;

    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) // ;
        return false;
    
    if (token->type != SEMICOLON) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
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
    
    T_TOKEN *token;
    // Create new symbol data
    T_SYMBOL_DATA data;
    data.func.return_type = VAR_VOID;
    data.func.argc = 0;
    data.func.argv = NULL;

    if (!get_save_token(buffer, &token)) 
        return false; // pub
    if (token->type != PUB) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) 
        return false; // fn
    if (token->type != FN) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!get_save_token(buffer, &token)) 
        return false; // identifier
    if (token->type != IDENTIFIER) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // save function name
    char *fn_name = token->lexeme;

    if (!get_save_token(buffer, &token)) 
        return false; // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_fp_params(buffer, &data)) { // PARAMS
        return false;
    }

    if (!get_save_token(buffer, &token)) { // )
        if (data.func.argv != NULL) {
            free(data.func.argv);
        }
        return false; 
    }

    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        if (data.func.argv != NULL) {
            free(data.func.argv);
        }
        return false;
    }

    if (!syntax_fp_fn_def_remaining(buffer, &data)) { // FN_DEF_REMAINING
        if (data.func.argv != NULL) {
            free(data.func.argv);
        }
        return false;
    }

    // Add function to symtable if it does not exist
    if (symtable_find_symbol(ST, fn_name) == NULL) {
        if (!symtable_add_symbol(ST, fn_name, SYM_FUNC, data)) {
            error_flag_fp = RET_VAL_INTERNAL_ERR;
            if (data.func.argv != NULL) {
                free(data.func.argv);
            }
            return false;
        }
    }
    else {
        error_flag_fp = RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR;
        if (data.func.argv != NULL) {
            free(data.func.argv);
        }
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
 * 
 * - `bool needs_last_token`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_fn_def_next(T_TOKEN_BUFFER *buffer) {
    
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
 * 
 * - `bool needs_last_token`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_fn_def_remaining(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data) {

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
        // save return type
        if (!syntax_fp_type(buffer, &(data->func.return_type))) { // TYPE
            return false;
        }

        if (!get_save_token(buffer, &token)) 
        return false; // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag_fp = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!simulate_fn_body(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        return true;
    }

    // second branch -> void { CODE_BLOCK_NEXT }
    if (token->type == VOID) { // void

        if (!get_save_token(buffer, &token)) 
        return false; // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag_fp = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!simulate_fn_body(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }
        // Set return type to void
        data->func.return_type = VAR_VOID;

        return true;
    }

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
 * 
 * - `bool needs_last_token`
 * @param *token_buffer pointer to token buffer
 * @param *data pointer to symbol data
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_params(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data) {
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

        if (!syntax_fp_param(buffer, data)) { // PARAM
            return false;
        }
        if (!syntax_fp_param_next(buffer, data)) { // PARAM_NEXT
            return false;
        }

        return true;
    }
    
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
bool syntax_fp_param(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data) {
    T_TOKEN *token;
    T_PARAM param;
    param.name = NULL;
    param.type = VAR_VOID;

    if (!get_save_token(buffer, &token)) 
        return false; // identifier
    if (token->type != IDENTIFIER) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    param.name = token->lexeme;

    if (!get_save_token(buffer, &token)) 
        return false; // :
    if (token->type != COLON) {
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_fp_type(buffer, &(param.type))) { // TYPE
        return false;
    }

    // Add parameter to the symbol data
    error_flag_fp = add_param_to_symbol_data(data, param);
    if (error_flag_fp != RET_VAL_OK) {
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
 * @param *type pointer to variable type
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_type(T_TOKEN_BUFFER *buffer, VAR_TYPE *type) {
    T_TOKEN *token;

    // check whether token is one of the types
    if (!get_save_token(buffer, &token)) 
        return false;
    if (token->type != TYPE_INT && token->type != TYPE_FLOAT &&
        token->type != TYPE_STRING && token->type != TYPE_INT_NULL && 
        token->type != TYPE_FLOAT_NULL && token->type != TYPE_STRING_NULL) {

            error_flag_fp = RET_VAL_SYNTAX_ERR;
            return false;
    }

    // Set the type
    switch (token->type) {
        case TYPE_INT:
            *type = VAR_INT;
            break;
        case TYPE_FLOAT:
            *type = VAR_FLOAT;
            break;
        case TYPE_STRING:
            *type = VAR_STRING;
            break;
        case TYPE_INT_NULL:
            *type = VAR_INT_NULL;
            break;
        case TYPE_FLOAT_NULL:
            *type = VAR_FLOAT_NULL;
            break;
        case TYPE_STRING_NULL:
            *type = VAR_STRING_NULL;
            break;
        default:
            // we should not reach this point
            break;
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
 * 
 * - `bool needs_last_token`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_param_next(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data) {
    
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

        if (!syntax_fp_param_after_comma(buffer, data)) { // PARAM_AFTER_COMMA
            return false;
        }

        return true;
    }

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
 * 
 * - `bool needs_last_token`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_fp_param_after_comma(T_TOKEN_BUFFER *buffer, T_SYMBOL_DATA *data) {
    
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
        
        if (!syntax_fp_param(buffer, data)) { // PARAM
            return false;
        }
        if (!syntax_fp_param_next(buffer, data)) { // PARAM_NEXT
            return false;
        }

        return true;
    }

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
        error_flag_fp = RET_VAL_SYNTAX_ERR;
        return false;
    }

    return true;
}

/**
 * @brief Simulates function body.
 * 
 * This function simulates the function body by counting curly brackets.
 * All tokens are stored in the token buffer.
 * 
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool simulate_fn_body(T_TOKEN_BUFFER *buffer) {

    int bracket_count = 1; // to correctly find function end
    T_TOKEN *token;

    while (true) {
        if (!get_save_token(buffer, &token)) { // get token
            return false;
        }
        
        // check for brackets
        switch (token->type) {
            case BRACKET_LEFT_CURLY:
                bracket_count++;
                break;

            case BRACKET_RIGHT_CURLY:
                bracket_count--;
                break;

            case EOF_TOKEN:
                error_flag_fp = RET_VAL_SYNTAX_ERR;
                return false;
            default:
                break;
        }

        // found correct end of function
        if (bracket_count == 0) {
            return true;
        }
    }

    // should not reach this point
    return false;
}