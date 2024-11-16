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

#include "first_phase.h"

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
    // TODO: add detection of function headers and store their signatures in the symtable


    // Fill the buffer with tokens
    while (true) {

        // Allocate memory for the token
        T_TOKEN *currentToken = (T_TOKEN *) malloc(sizeof(T_TOKEN));
        if (currentToken == NULL) {
            return RET_VAL_INTERNAL_ERR;
        }
        
        // Get the next token
        int err_code = get_token(currentToken);
        if (err_code != 0){
            return RET_VAL_LEXICAL_ERR;
        }

        // Add the token to the buffer
        if (!add_token_as_last(token_buffer, currentToken)) {
            return RET_VAL_INTERNAL_ERR;
        }

        if (currentToken->type == EOF_TOKEN){
            break;
        }
    }

    return RET_VAL_OK;
}