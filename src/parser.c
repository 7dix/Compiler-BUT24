// FILE: parser.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
// NOTES: Syntax-driven compilation for the IFJ24 language.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "parser.h"
#include "return_values.h"
#include "token_buffer.h"
#include "symtable.h"
#include "semantic.h"
#include "precedence.h"
#include "precedence_tree.h"
#include "gen_handler.h"

//--------------------------- GLOBAL VARIABLES ----------------------------//

// Stores any error generated during this phase, see `return_values.h`
T_RET_VAL error_flag = RET_VAL_OK;


//------------------ PRIVATE FUNCTION PROTOTYPES --------------------------//

bool is_token_in_expr(T_TOKEN *token);
bool syntax_start(T_TOKEN_BUFFER *token_buffer);
bool syntax_prolog(T_TOKEN_BUFFER *buffer);
bool syntax_fn_def(T_TOKEN_BUFFER *buffer);
bool syntax_fn_def_next(T_TOKEN_BUFFER *buffer);
bool syntax_fn_def_remaining(T_TOKEN_BUFFER *buffer);
bool syntax_params(T_TOKEN_BUFFER *buffer);
bool syntax_param(T_TOKEN_BUFFER *buffer);
bool syntax_type(T_TOKEN_BUFFER *buffer, VarType *type);
bool syntax_param_next(T_TOKEN_BUFFER *buffer);
bool syntax_param_after_comma(T_TOKEN_BUFFER *buffer);
bool syntax_end(T_TOKEN_BUFFER *buffer);
bool syntax_code_block_next(T_TOKEN_BUFFER *buffer);
bool syntax_code_block(T_TOKEN_BUFFER *buffer);
bool syntax_var_def(T_TOKEN_BUFFER *buffer);
bool syntax_var_def_after_id(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_if_statement(T_TOKEN_BUFFER *buffer);
bool syntax_if_statement_remaining(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree);
bool syntax_while_statement(T_TOKEN_BUFFER *buffer);
bool syntax_while_statement_remaining(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree);
bool syntax_return(T_TOKEN_BUFFER *buffer);
bool syntax_return_remaining(T_TOKEN_BUFFER *buffer);
bool syntax_built_in_void_fn_call(T_TOKEN_BUFFER *buffer);
bool syntax_assign_expr_or_fn_call(T_TOKEN_BUFFER *buffer);
bool syntax_assign_discard_expr_or_fn_call(T_TOKEN_BUFFER *buffer);
bool syntax_id_start(T_TOKEN_BUFFER *buffer, Symbol *symbol);
bool syntax_function_arguments(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_assign(T_TOKEN_BUFFER *buffer, SymbolData *data);
bool syntax_id_assign(T_TOKEN_BUFFER *buffer, SymbolData *data, char *id_name);
bool syntax_arguments(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_argument_next(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_argument_after_comma(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);
bool syntax_argument(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call);

/**
 * @brief Entry point of the parser.
 * 
 * Runs full syntax-driven compilation. Uses supplied token buffer.
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @note TODO: add semantic checks, cleaning, etc.
 * @note TODO: improve comments on return values
 * @param *token_buffer pointer to token buffer
 * @return `int`
 * @retval `RET_VAL_OK` if parsing was successful
 * @retval `RET_VAL_SYNTAX_ERR` if syntax error occurred
 * @retval `RET_VAL_INTERNAL_ERR` if internal error occurred
 */
int run_parser(T_TOKEN_BUFFER *token_buffer) {
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
                token->type == BRACKET_RIGHT_SIMPLE || token->type == IDENTIFIER ||
                token->type == NULL_TOKEN || token->type == STRING);
}

/**
 * @brief Checks whether given token is a relation operator.
 * 
 * @param *token pointer to token
 * @return `bool`
 * @retval `true` - if token is a relation operator
 * @retval `false` - otherwise
 */
bool is_token_relation_operator(T_TOKEN *token) {
    return (    token->type == NOT_EQUAL || token->type == LESS_THAN ||
                token->type == GREATER_THAN || token->type == EQUAL ||
                token->type == LESS_THAN_EQUAL || token->type == GREATER_THAN_EQUAL );
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
bool syntax_start(T_TOKEN_BUFFER *token_buffer) {

    if (!syntax_prolog(token_buffer)) { // PROLOG
        return false;
    }
    if (!syntax_fn_def_next(token_buffer)) { // FN_DEF_NEXT
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
    T_TOKEN *token;

    next_token(buffer, &token); // const
    if (token->type != CONST) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ifj
    if (token->type != IFJ) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // =
    if (token->type != ASSIGN) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    next_token(buffer, &token); // @import
    if (token->type != IMPORT) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }
    
    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // string
    if (token->type != STRING) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // check that string is equal to "ifj24.zig"
    if (strcmp(token->value.stringVal, "ifj24.zig") != 0) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ;
    if (token->type != SEMICOLON) {
        error_flag = RET_VAL_SYNTAX_ERR;
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

    T_TOKEN *token;

    next_token(buffer, &token); // pub
    if (token->type != PUB) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // fn
    if (token->type != FN) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // save current function name
    set_fn_name(ST, token->lexeme);


    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_params(buffer)) { // PARAMS
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_fn_def_remaining(buffer)) { // FN_DEF_REMAINING
        return false;
    }

    // reset current function name
    set_fn_name(ST, NULL);

    // CD: generate implicit return, needs to be changed in the future
    createReturn();

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
    // TODO: add cleaning, etc.
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
        VarType type_void; // stores function return type
        if (!syntax_type(buffer, &type_void)) { // TYPE
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // SCOPE INCREASE
        if (!symtable_add_scope(ST, false)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false; 
        }
        error_flag = put_param_to_symtable(get_fn_name(ST));
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        // CD: generate function header
        createFnHeader(get_fn_name(ST));

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        // SCOPE DECREASE, check for unused variables
        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> void { CODE_BLOCK_NEXT }
    if (token->type == VOID) { // void

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // SCOPE INCREASE
        if (!symtable_add_scope(ST, false)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }
        error_flag = put_param_to_symtable(get_fn_name(ST));
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        // CD: generate function header
        createFnHeader(get_fn_name(ST));

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            return false;
        }

        // SCOPE DECREASE, check for unused variables
        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

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

    T_TOKEN *token;

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // :
    if (token->type != COLON) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    VarType type_void; // dummy variable
    if (!syntax_type(buffer, &type_void)) { // TYPE
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
 * @param *type pointer to variable type
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_type(T_TOKEN_BUFFER *buffer, VarType *type) {

    T_TOKEN *token;
    // check whether token is one of the types
    next_token(buffer, &token);
    if (token->type != TYPE_INT && token->type != TYPE_FLOAT &&
        token->type != TYPE_STRING && token->type != TYPE_INT_NULL && 
        token->type != TYPE_FLOAT_NULL && token->type != TYPE_STRING_NULL) {
        
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
    }
    
    // Set the type
    switch(token->type) {
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
            // We should never get here
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
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_param_next(T_TOKEN_BUFFER *buffer) {

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

    // Create new symbol data
    char *name = NULL;
    SymbolData data;
    data.var.is_const = false;
    data.var.modified = false;
    data.var.used = false;
    data.var.const_expr = false;
    data.var.type = VAR_VOID;
    data.var.id = -1;
    data.var.float_value = 0.0;

    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> const identifier VAR_DEF_AFTER_ID
    if (token->type == CONST) { // const
        data.var.is_const = true;
        data.var.modified = true;
        data.var.used = false;

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // save variable name
        name = token->lexeme;
        // Check if variable already exists
        if (symtable_find_symbol(ST, name) != NULL) {
            error_flag = RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR;
            return false;
        }

        // here is done everything related to type and assignment
        if (!syntax_var_def_after_id(buffer, &data)) { // VAR_DEF_AFTER_ID
            return false;
        }

        // TODO: need to check if this is correct based on right side of assignment
        // Check if variable type was set
        if (data.var.type == VAR_VOID) {
            error_flag = RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR;
            return false;
        }

        // Add variable to symtable
        if (!symtable_add_symbol(ST, name, SYM_VAR, data)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        // CD: generate variable definition
        handleUniqDefvar(token);
        // CD: generate mov for right side
        handleAssign(name);

        return true;
    }

    // second branch -> var identifier VAR_DEF_AFTER_ID
    if (token->type == VAR) { // var
        data.var.is_const = false;
        data.var.modified = false;
        data.var.used = false;
        
        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // save variable name
        name = token->lexeme;
        // Check if variable already exists
        if (symtable_find_symbol(ST, name) != NULL) {
            error_flag = RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR;
            return false;
        }

        // here is done everything related to type and assignment
        if (!syntax_var_def_after_id(buffer, &data)) { // VAR_DEF_AFTER_ID
            return false;
        }

        // TODO: need to check if this is correct based on right side of assignment
        // Check if variable type was set
        if (data.var.type == VAR_VOID) {
            error_flag = RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR;
            return false;
        }

        // Add variable to symtable
        if (!symtable_add_symbol(ST, name, SYM_VAR, data)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        // codegen print var definition
        handleUniqDefvar(token);
        // codegen print mov for right side
        handleAssign(name);

        return true;
    }

    // we should not reach this point, anyway ...
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
 * @param *data pointer to symbol data
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_var_def_after_id(T_TOKEN_BUFFER *buffer, SymbolData *data) {

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> : TYPE = ASSIGN
    if (token->type == COLON) { // :

        if (!syntax_type(buffer, &(data->var.type))) { // TYPE
            return false;
        }

        next_token(buffer, &token);
        if (token->type != ASSIGN) { // =
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_assign(buffer, data)) { // ASSIGN
            return false;
        }
    
        return true;
    }

    // second branch -> = ASSIGN
    if (token->type == ASSIGN) { // =

        if (!syntax_assign(buffer, data)) { // ASSIGN
            return false;
        }

        return true;
    }

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

    T_TOKEN *token;

    next_token(buffer, &token); // if
    if (token->type != IF) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
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

    // get expression type
    error_flag = check_expression(ST, &tree);
    if (error_flag != 0){
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_if_statement_remaining(buffer, &tree)) { // IF_STATEMENT_REMAINING
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
 * @param *tree pointer to expression tree
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_if_statement_remaining(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree) {

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
    if (token->type == BRACKET_LEFT_CURLY) { // {
        
        if ((*tree)->resultType != TYPE_BOOL_RESULT) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        // CD: generate expression
        createStackByPostorder(*tree);

        tree_dispose(tree);

        int fc_defined_upper = is_in_fc(ST);

        if (!symtable_add_scope(ST, true)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        char *labelElse = NULL;
        char *labelEnd = NULL;

        if (!generate_labels(ST,&labelElse, &labelEnd)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        int fc_defined_if = is_in_fc(ST);

        handleIfStartBool(labelElse, fc_defined_upper, fc_defined_if);

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            free(labelElse);
            free(labelEnd);
            return false;
        }

        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            free(labelElse);
            free(labelEnd);
            return false;
        }
        if (!symtable_add_scope(ST, true)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        int fc_defined_else = is_in_fc(ST);

        createIfElse(labelEnd, labelElse, fc_defined_upper, fc_defined_if, fc_defined_else);

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        next_token(buffer, &token); // else
        if (token->type != ELSE) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            free(labelElse);
            free(labelEnd);
            return false;
        }

        createIfEnd(labelEnd, fc_defined_else);

        free(labelElse);
        free(labelEnd);

        // SCOPE DECREASE
        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> | identifier | { CODE_BLOCK_NEXT } else { CODE_BLOCK_NEXT }
    if (token->type == PIPE) { // |
        
        if (!is_result_type_nullable((*tree)->resultType)) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        SymbolData data;
        data.var.type = fc_nullable_convert_type((*tree)->resultType);
        // TODO: check behaviour of VAR_NULL !!
        if (data.var.type == VAR_VOID) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        // CD: generate expression

        createStackByPostorder(*tree);

        tree_dispose(tree);

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        int fc_defined_upper = is_in_fc(ST);

        // SCOPE INCREASE
        if (!symtable_add_scope(ST, true)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false; 
        }

        char *labelElse = NULL;
        char *labelEnd = NULL;
        if (!generate_labels(ST,&labelElse, &labelEnd)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        // Add the | identifier | to the symtable
        data.var.is_const = true;
        data.var.modified = true;
        data.var.used = false;
        data.var.id = -1;

        if (symtable_find_symbol(ST, token->lexeme) != NULL) {
            error_flag = RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        if (!symtable_add_symbol(ST, token->lexeme, SYM_VAR, data)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        int fc_defined_if = is_in_fc(ST);

        // CD: generate if nullable header
        handleIfStartNil(labelElse, token, fc_defined_upper, fc_defined_if);

        next_token(buffer, &token); // |
        if (token->type != PIPE) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            free(labelElse);
            free(labelEnd);
            return false;
        }

        // SCOPE DECREASE
        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            free(labelElse);
            free(labelEnd);
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        next_token(buffer, &token); // else
        if (token->type != ELSE) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelElse);
            free(labelEnd);
            return false;
        }

        // SCOPE INCREASE
        if (!symtable_add_scope(ST, true)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            free(labelElse);
            free(labelEnd);
            return false; 
        }

        int fc_defined_else = is_in_fc(ST);

        // CD: generate if nullable else
        createIfElse(labelEnd, labelElse, fc_defined_upper, fc_defined_if, fc_defined_else);

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            free(labelElse);
            free(labelEnd);
            return false;
        }

        // SCOPE DECREASE
        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            free(labelElse);
            free(labelEnd);
            return false;
        }

        // CD: generate if nullable end
        createIfEnd(labelEnd, fc_defined_else);
        free(labelElse);
        free(labelEnd);

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

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

    T_TOKEN *token;

    next_token(buffer, &token); // while
    if (token->type != WHILE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
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

    // get expression type
    error_flag = check_expression(ST, &tree);
    if (error_flag != 0){
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_while_statement_remaining(buffer, &tree)) { // WHILE_STATEMENT_REMAINING
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
bool syntax_while_statement_remaining(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree) {

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> { CODE_BLOCK_NEXT }
    if (token->type == BRACKET_LEFT_CURLY) { // {

        if ((*tree)->resultType != TYPE_BOOL_RESULT) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        char *labelStart = NULL;
        char *labelEnd = NULL;
        if (!generate_labels(ST,&labelStart, &labelEnd)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }


        int fc_defined_upper = is_in_fc(ST);

        // SCOPE INCREASE
        if (!symtable_add_scope(ST, true)) {
            free(labelStart);
            free(labelEnd);
            error_flag = RET_VAL_INTERNAL_ERR;
            return false; 
        }

        int fc_defined_current = is_in_fc(ST);

        // CD: generate while start
        createWhileBoolHeader(labelStart, fc_defined_upper, fc_defined_current);

        // CD: generate expression
        createStackByPostorder(*tree);

        // CD: generate while condition
        handleWhileBool(labelEnd);

        tree_dispose(tree);

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            free(labelStart);
            free(labelEnd);
            return false;
        }

        // CD: generate while end
        createWhileEnd(labelStart, labelEnd, fc_defined_current);
        free(labelStart);
        free(labelEnd);

        // SCOPE DECREASE
        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> | identifier | { CODE_BLOCK_NEXT }
    if (token->type == PIPE) { // |

        if (!is_result_type_nullable((*tree)->resultType)) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        SymbolData data;
        data.var.type = fc_nullable_convert_type((*tree)->resultType);
        // TODO: check behaviour of VAR_NULL !!
        if (data.var.type == VAR_VOID) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (symtable_find_symbol(ST, token->lexeme) != NULL) {
            error_flag = RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR;
            return false;
        }

        int fc_defined_upper = is_in_fc(ST);

        // SCOPE INCREASE
        if (!symtable_add_scope(ST, true)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false; 
        }

        int fc_defined_current = is_in_fc(ST);

        // Add the | identifier | to the symtable
        data.var.is_const = true; // TODO: check corectness
        data.var.modified = true; // TODO: check corectness
        data.var.used = false;
        data.var.id = -1;
        if (!symtable_add_symbol(ST, token->lexeme, SYM_VAR, data)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        char *labelStart = NULL;
        char *labelEnd = NULL;
        if (!generate_labels(ST,&labelStart, &labelEnd)) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        // CD: generate while start
        createWhileNilHeader(labelStart, token, fc_defined_upper, fc_defined_current);

        // CD: generate expression
        createStackByPostorder(*tree);

        // CD: generate while condition
        handleWhileNil(labelEnd, token);

        tree_dispose(tree);

        next_token(buffer, &token); // |
        if (token->type != PIPE) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelStart);
            free(labelEnd);
            return false;
        }

        next_token(buffer, &token); // {
        if (token->type != BRACKET_LEFT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            free(labelStart);
            free(labelEnd);
            return false;
        }

        if (!syntax_code_block_next(buffer)) { // CODE_BLOCK_NEXT
            free(labelStart);
            free(labelEnd);
            return false;
        }

        // CD: generate while end
        createWhileEnd(labelStart, labelEnd, fc_defined_current);
        free(labelStart);
        free(labelEnd);

        // SCOPE DECREASE
        error_flag = symtable_remove_scope(ST, true);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        next_token(buffer, &token); // }
        if (token->type != BRACKET_RIGHT_CURLY) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

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

    T_TOKEN *token;

    next_token(buffer, &token); // return
    if (token->type != RETURN) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_return_remaining(buffer)) { // RETURN_REMAINING
        return false;
    }

    // CD: generate return
    createReturn();

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

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> ;
    if (token->type == SEMICOLON) { // ;
        // Check if the function is void
        if (symtable_find_symbol(ST, get_fn_name(ST))->data.func.return_type != VAR_VOID) {
            error_flag = RET_VAL_SEMANTIC_FUNC_RETURN_ERR;
            return false;
        }

        return true;
    }

    // second branch -> ASSIGN, check first token in ASSIGN
    if (token->type == IDENTIFIER || token->type == IFJ || is_token_in_expr(token)) {
        move_back(buffer); // token needed in ASSIGN

        // Dummy for return type
        SymbolData data;

        Symbol *symbol = symtable_find_symbol(ST, get_fn_name(ST));
        if (symbol == NULL) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }

        data.var.type = symbol->data.func.return_type;

        // handling of expression
        if (!syntax_assign(buffer, &data)) { // ASSIGN
            return false;
        }

        return true;
    }

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
    T_TOKEN *token;
    T_FN_CALL fn_call;
    fn_call.argv = NULL;
    fn_call.argc = 0;

    next_token(buffer, &token); // ifj
    if (token->type != IFJ) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // .
    if (token->type != DOT) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // Check if the function is defined
    char *fn_name = NULL;
    size_t len = snprintf(fn_name, 0, "ifj.%s", token->lexeme);
    fn_name = malloc(sizeof(char) * (len + 1));
    if (fn_name == NULL) {
        error_flag = RET_VAL_INTERNAL_ERR;
        return false;
    }
    sprintf(fn_name, "ifj.%s", token->lexeme);

    Symbol *symbol = symtable_find_symbol(ST, fn_name);
    if (symbol == NULL || symbol->type != SYM_FUNC) {
        error_flag = RET_VAL_SEMANTIC_UNDEFINED_ERR;
        return false;
    }

    // Set the function call name
    fn_call.name = fn_name;
    fn_call.ret_type = symbol->data.func.return_type;

    // check function return type is void
    if (fn_call.ret_type != VAR_VOID) {
        error_flag = RET_VAL_SEMANTIC_FUNCTION_ERR;
        return false;
    }

    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_arguments(buffer, &fn_call)) { // ARGUMENTS
        return false;
    }

    // Check if the function call is correct
    error_flag = check_function_call(ST, &fn_call);
    if (error_flag != RET_VAL_OK) {
        return false;
    }

    // CD: generate built-in function call
    callBIFn(&fn_call);

    free(fn_name);

    free_fn_call_args(&fn_call);

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ;
    if (token->type != SEMICOLON) {
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
    
    T_TOKEN *token;

    next_token(buffer, &token); // identifier
    if (token->type != IDENTIFIER) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    Symbol *symbol = symtable_find_symbol(ST, token->lexeme);

    // Check if the identifier is defined
    if (symbol == NULL) {
        error_flag = RET_VAL_SEMANTIC_UNDEFINED_ERR;
        return false;
    }

    if (!syntax_id_start(buffer, symbol)) { // ID_START
        return false;
    }

    // codegen print mov for right side
    if (symbol->type == SYM_VAR) {
        handleAssign(token->lexeme);
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

    T_TOKEN *token;

    next_token(buffer, &token); // discard_identifier
    if (token->type != IDENTIFIER_DISCARD) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // =
    if (token->type != ASSIGN) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // Dummy for return type
    SymbolData data;
    data.var.type = VAR_VOID;

    // handling of expression
    if (!syntax_assign(buffer, &data)) { // ASSIGN
        return false;
    }

    // CD: generate discard
    handleDiscard();

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
 * @param *symbol pointer to symbol
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_id_start(T_TOKEN_BUFFER *buffer, Symbol *symbol) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have two branches, choose here
    next_token(buffer, &token);
    // first branch -> = ASSIGN
    if (token->type == ASSIGN) { // =
        if (symbol->type != SYM_VAR) {
            error_flag = RET_VAL_SEMANTIC_UNDEFINED_ERR; // TODO: correct error code?
            return false;
        }
        if (symbol->data.var.is_const) {
            error_flag = RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR;
            return false;
        }
        
        symbol->data.var.modified = true;
        symbol->data.var.used = true;

        if (!syntax_assign(buffer, &(symbol->data))) { // ASSIGN
            return false;
        }

        // TODO: maybe check if the varType is not void

        return true;
    }

    // void function call to a user defined function
    // second branch -> FUNCTION_ARGUMENTS
    if (token->type == BRACKET_LEFT_SIMPLE) { // (
        if (symbol->type != SYM_FUNC) {
            error_flag = RET_VAL_SEMANTIC_UNDEFINED_ERR; // TODO: correct error code?
            return false;
        }
        
        T_FN_CALL fn_call;
        fn_call.name = symbol->name;
        fn_call.ret_type = symbol->data.func.return_type;
        fn_call.argv = NULL;
        fn_call.argc = 0;

        move_back(buffer); // token needed in FUNCTION_ARGUMENTS
        if (!syntax_function_arguments(buffer, &fn_call)) { // FUNCTION_ARGUMENTS
            return false;
        }

        // Check if the function call is correct
        error_flag = check_function_call(ST, &fn_call);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        // check function is non-void
        if (fn_call.ret_type != VAR_VOID) {
            error_flag = RET_VAL_SEMANTIC_FUNCTION_ERR;
            return false;
        }

        // CD: generate function call
        callFunction(&fn_call);

        free_fn_call_args(&fn_call);

        return true;
    }

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
bool syntax_function_arguments(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call) {

    T_TOKEN *token;
    
    next_token(buffer, &token); // (
    if (token->type != BRACKET_LEFT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    if (!syntax_arguments(buffer, fn_call)) { // ARGUMENTS
        return false;
    }

    next_token(buffer, &token); // )
    if (token->type != BRACKET_RIGHT_SIMPLE) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    next_token(buffer, &token); // ;
    if (token->type != SEMICOLON) {
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
 * @param *data pointer to symbol data
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_assign(T_TOKEN_BUFFER *buffer, SymbolData *data) {
    T_TOKEN *token;

    // set return flag for error handling based previous token
    bool is_return = false;

    move_back(buffer);
    next_token(buffer, &token);
    if (token->type == RETURN) {
        is_return = true;
    }



    // we have several branches, choose here
    next_token(buffer, &token);
    
    // first branch -> ifj . identifier ( ARGUMENTS ) ;
    if (token->type == IFJ) { // ifj

        T_FN_CALL fn_call;
        fn_call.argv = NULL;
        fn_call.argc = 0;

        next_token(buffer, &token); // .
        if (token->type != DOT) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // identifier
        if (token->type != IDENTIFIER) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        // Check if the function is defined and check the return type
        char *fn_name = NULL;
        size_t len = snprintf(fn_name, 0, "ifj.%s", token->lexeme);
        fn_name = malloc(sizeof(char) * (len + 1));
        if (fn_name == NULL) {
            error_flag = RET_VAL_INTERNAL_ERR;
            return false;
        }
        sprintf(fn_name, "ifj.%s", token->lexeme);

        Symbol *symbol = symtable_find_symbol(ST, fn_name);
        if (symbol == NULL || symbol->type != SYM_FUNC) {
            error_flag = RET_VAL_SEMANTIC_UNDEFINED_ERR;
            return false;
        }
        // Set the function call name
        fn_call.name = fn_name;
        fn_call.ret_type = symbol->data.func.return_type;



        if (compare_var_types(&(data->var.type), &(symbol->data.func.return_type)) != 0) {
            error_flag = RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR;
            return false;
        }


        next_token(buffer, &token); // (
        if (token->type != BRACKET_LEFT_SIMPLE) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        if (!syntax_arguments(buffer, &fn_call)) { // ARGUMENTS
            return false;
        }

        // Check if the function is void
        error_flag = check_function_call(ST, &fn_call);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        // CD: generate built-in function call
        callBIFn(&fn_call);

        free(fn_name);

        free_fn_call_args(&fn_call);


        next_token(buffer, &token); // )
        if (token->type != BRACKET_RIGHT_SIMPLE) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        next_token(buffer, &token); // ;
        if (token->type != SEMICOLON) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

    // second branch -> identifier ID_ASSIGN
    if (token->type == IDENTIFIER) { // identifier

        if (!syntax_id_assign(buffer, data, token->lexeme)) { // ID_ASSIGN
            return false;
        }

        return true;
    }

    // third branch -> EXPRESSION ;
    if (is_token_in_expr(token)) {
        move_back(buffer);

        T_TREE_NODE_PTR tree;
        tree_init(&tree);
        error_flag = precedenceSyntaxMain(buffer, &tree, ASS_END);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        error_flag = check_expression(ST, &tree);
        if (error_flag != RET_VAL_OK){
            return false;
        }

        // tree->resultType
        VarType exprRes = VAR_VOID;
        switch (tree->resultType) {
            case TYPE_NULL_RESULT:
                exprRes = VAR_NULL;
                break;
            case TYPE_BOOL_RESULT:
                exprRes = VAR_BOOL;
                break;
            case TYPE_INT_RESULT:
                exprRes = VAR_INT;
                break;
            case TYPE_FLOAT_RESULT:
                exprRes = VAR_FLOAT;
                if (data->var.is_const) {
                    data->var.const_expr = true;
                    data->var.float_value = tree->token->value.floatVal;
                }
                break;
            case TYPE_INT_NULL_RESULT:
                exprRes = VAR_INT_NULL;
                break;
            case TYPE_FLOAT_NULL_RESULT:
                exprRes = VAR_FLOAT_NULL;
                break;
            case TYPE_STRING_RESULT:
                exprRes = VAR_STRING;
                break;
            case TYPE_STRING_NULL_RESULT:
                exprRes = VAR_STRING_NULL;
                break;
            case TYPE_STRING_LITERAL_RESULT:
                exprRes = STRING_LITERAL;
                break;
            case TYPE_NOTSET_RESULT:
            default:
                tree_dispose(&tree);
                error_flag = RET_VAL_INTERNAL_ERR;
                return false;
        }

        if (compare_var_types(&(data->var.type), &exprRes) != 0) {
            if (!is_return) {
                error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
                return false;
            }
            else {
                // Handling for return type
                error_flag = RET_VAL_SEMANTIC_FUNCTION_ERR;
                return false;
            }
        }

        // CD: generate expression
        createStackByPostorder(tree);

        tree_dispose(&tree);

        next_token(buffer, &token); // ;
        if (token->type != SEMICOLON) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }
        
        return true;
    }

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
 * `ID_ASSIGN -> ;` - special case only one variable in assignment
 * 
 * `ID_ASSIGN -> FUNCTION_ARGUMENTS`
 * 
 * This function uses following global variables:
 * 
 * - `int error_flag`
 * @param *token_buffer pointer to token buffer
 * @param *data symbol data
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_id_assign(T_TOKEN_BUFFER *buffer, SymbolData *data, char *id_name) {
    // TODO: add semantic checks, cleaning, etc.

    T_TOKEN *token;
    // we have three branches, choose here
    next_token(buffer, &token);
    // first branch -> FUNCTION_ARGUMENTS
    if (token->type == BRACKET_LEFT_SIMPLE) { // (
        move_back(buffer);
        T_FN_CALL fn_call;
        fn_call.argv = NULL;
        fn_call.argc = 0;
        fn_call.name = id_name;

        // Check if the function is defined
        Symbol *symbol = symtable_find_symbol(ST, fn_call.name);
        if (symbol == NULL || symbol->type != SYM_FUNC) {
            error_flag = RET_VAL_SEMANTIC_UNDEFINED_ERR;
            return false;
        }

        fn_call.ret_type = symbol->data.func.return_type;

        if (compare_var_types(&(data->var.type), &(fn_call.ret_type)) != 0) {
            error_flag = RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR;
            return false;
        }

        if (!syntax_function_arguments(buffer, &fn_call)) { // FUNCTION_ARGUMENTS
            return false;
        }

        // Check if the function is void
        error_flag = check_function_call(ST, &fn_call);
        if (error_flag != RET_VAL_OK) {
            return false;
        }
        // CD: generate function call
        callFunction(&fn_call);

        free_fn_call_args(&fn_call);

        return true;
    }

    // second branch -> ; just a variable in assignment
    if (token->type == SEMICOLON) { // ;
        move_back(buffer);
        move_back(buffer);

        T_TREE_NODE_PTR tree;
        tree_init(&tree);
        error_flag = precedenceSyntaxMain(buffer, &tree, ASS_END);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        // get expression type
        error_flag = check_expression(ST, &tree);
        if (error_flag != 0){
            return false;
        }

        // tree->resultType
        VarType exprRes = VAR_VOID;
        switch (tree->resultType) {
            case TYPE_NULL_RESULT:
                exprRes = VAR_NULL;
                break;
            case TYPE_BOOL_RESULT:
                exprRes = VAR_BOOL;
                break;
            case TYPE_INT_RESULT:
                exprRes = VAR_INT;
                break;
            case TYPE_FLOAT_RESULT:
                exprRes = VAR_FLOAT;
                if (data->var.is_const) {
                    data->var.const_expr = true;
                    data->var.float_value = tree->token->value.floatVal;
                }
                break;
            case TYPE_INT_NULL_RESULT:
                exprRes = VAR_INT_NULL;
                break;
            case TYPE_FLOAT_NULL_RESULT:
                exprRes = VAR_FLOAT_NULL;
                break;
            case TYPE_STRING_RESULT:
                exprRes = VAR_STRING;
                break;
            case TYPE_STRING_NULL_RESULT:
                exprRes = VAR_STRING_NULL;
                break;
            case TYPE_STRING_LITERAL_RESULT:
                exprRes = STRING_LITERAL;
                break;
            case TYPE_NOTSET_RESULT:
            default:
                tree_dispose(&tree);
                error_flag = RET_VAL_INTERNAL_ERR;
                return false;
        }

        if (compare_var_types(&(data->var.type), &exprRes) != 0) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        // CD: generate expression
        createStackByPostorder(tree);

        tree_dispose(&tree);
        

        next_token(buffer, &token);
        if (token->type == SEMICOLON) {
            return true;
        }

        return true;
    }

    // third branch -> EXPRESSION ;
    if (is_token_in_expr(token)) { // is token in expression ?
        // we have to move back twice to get to the beginning of the expression
        // we ate the first token of the expression as identifier in ASSIGN non-terminal
        move_back(buffer);
        move_back(buffer);

        T_TREE_NODE_PTR tree;
        tree_init(&tree);
        error_flag = precedenceSyntaxMain(buffer, &tree, ASS_END);
        if (error_flag != RET_VAL_OK) {
            return false;
        }

        // get expression type
        error_flag = check_expression(ST, &tree);
        if (error_flag != 0){
            return false;
        }

        // tree->resultType
        VarType exprRes = VAR_VOID;
        switch (tree->resultType) {
            case TYPE_NULL_RESULT:
                exprRes = VAR_NULL;
                break;
            case TYPE_BOOL_RESULT:
                exprRes = VAR_BOOL;
                break;
            case TYPE_INT_RESULT:
                exprRes = VAR_INT;
                break;
            case TYPE_FLOAT_RESULT:
                exprRes = VAR_FLOAT;
                if (data->var.is_const) {
                    data->var.const_expr = true;
                    data->var.float_value = tree->token->value.floatVal;
                }
                break;
            case TYPE_INT_NULL_RESULT:
                exprRes = VAR_INT_NULL;
                break;
            case TYPE_FLOAT_NULL_RESULT:
                exprRes = VAR_FLOAT_NULL;
                break;
            case TYPE_STRING_RESULT:
                exprRes = VAR_STRING;
                break;
            case TYPE_STRING_NULL_RESULT:
                exprRes = VAR_STRING_NULL;
                break;
            case TYPE_STRING_LITERAL_RESULT:
                exprRes = STRING_LITERAL;
                break;
            case TYPE_NOTSET_RESULT:
            default:
                tree_dispose(&tree);
                error_flag = RET_VAL_INTERNAL_ERR;
                return false;
        }

        if (compare_var_types(&(data->var.type), &exprRes) != 0) {
            error_flag = RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR;
            return false;
        }

        // CD: generate expression
        createStackByPostorder(tree);

        tree_dispose(&tree);

        next_token(buffer, &token); // ;
        if (token->type != SEMICOLON) {
            error_flag = RET_VAL_SYNTAX_ERR;
            return false;
        }

        return true;
    }

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
 * @param *fn_call pointer to function call struct
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_arguments(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call) {

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

        if (!syntax_argument(buffer, fn_call)) { // ARGUMENT
            return false;
        }
        if (!syntax_argument_next(buffer, fn_call)) { // ARGUMENT_NEXT
            return false;
        }

        return true;
    }

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
 * @param *fn_call pointer to function call struct
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_argument_next(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call) {

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

        if (!syntax_argument_after_comma(buffer, fn_call)) { // ARGUMENT_AFTER_COMMA
            return false;
        }

        return true;
    }

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
 * @param *fn_call pointer to function call struct
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_argument_after_comma(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call) {

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

        if (!syntax_argument(buffer, fn_call)) { // ARGUMENT
            return false;
        }
        if (!syntax_argument_next(buffer, fn_call)) { // ARGUMENT_NEXT
            return false;
        }

        return true;
    }

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
 * @param *fn_call pointer to function call struct
 * @return `bool`
 * @retval `true` - correct syntax
 * @retval `false` - syntax error
 */
bool syntax_argument(T_TOKEN_BUFFER *buffer, T_FN_CALL *fn_call) {

    T_TOKEN *token;
    // check first terminal in argument -> identifier, int, float, string, null
    next_token(buffer, &token);
    if (token->type != IDENTIFIER && token->type != INT &&
        token->type != FLOAT && token->type != STRING && token->type != NULL_TOKEN) {
        error_flag = RET_VAL_SYNTAX_ERR;
        return false;
    }

    // Add the argument to the function call
    add_arg_to_fn_call(fn_call, token);

    return true;
}
