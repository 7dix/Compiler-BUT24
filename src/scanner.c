// FILE: scanner.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Marek Tenora> (xtenor02)
//
// YEAR: 2024
// NOTES: Implementation of the Lexical Analyzer (Scanner)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "shared.h"

// Global variables
static int line_number = 1;       // Current line number in the source file

// Function prototypes
int is_keyword(const char *lexeme);
TOKEN_TYPE get_keyword_token_type(const char *lexeme);
int is_type_identifier(const char *lexeme);
TOKEN_TYPE get_type_identifier_token_type(const char *lexeme);
void unget_char(int c);
int get_next_char();

// Buffer for lexeme construction
#define INITIAL_LEXEME_SIZE 128

/**
 * @brief Gets the string representation of a token type.
 *
 * @param type The token type.
 * @return const char* The string representation.
 */
const char* tt_to_str(TOKEN_TYPE type) {
    switch (type) {
        case IMPORT: return "IMPORT";
        case NOT_EQUAL: return "NOT_EQUAL";
        case STRING: return "STRING";
        case INT: return "INT";
        case FLOAT: return "FLOAT";
        case LESS_THAN: return "LESS_THAN";
        case GREATER_THAN: return "GREATER_THAN";
        case ASSIGN: return "ASSIGN";
        case EQUAL: return "EQUAL";
        case LESS_THAN_EQUAL: return "LESS_THAN_EQUAL";
        case GREATER_THAN_EQUAL: return "GREATER_THAN_EQUAL";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULTIPLY: return "MULTIPLY";
        case DIVIDE: return "DIVIDE";
        case COLON: return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case BRACKET_LEFT_SIMPLE: return "BRACKET_LEFT_SIMPLE";
        case BRACKET_RIGHT_SIMPLE: return "BRACKET_RIGHT_SIMPLE";
        case BRACKET_LEFT_CURLY: return "BRACKET_LEFT_CURLY";
        case BRACKET_RIGHT_CURLY: return "BRACKET_RIGHT_CURLY";
        case IDENTIFIER: return "IDENTIFIER";
        case IDENTIFIER_DISCARD: return "IDENTIFIER_DISCARD";
        case TYPE_INT: return "TYPE_INT";
        case TYPE_FLOAT: return "TYPE_FLOAT";
        case TYPE_STRING: return "TYPE_STRING";
        case TYPE_INT_NULL: return "TYPE_INT_NULL";
        case TYPE_FLOAT_NULL: return "TYPE_FLOAT_NULL";
        case TYPE_STRING_NULL: return "TYPE_STRING_NULL";
        case DOT: return "DOT";
        case COMMA: return "COMMA";
        case PIPE: return "PIPE";
        case CONST: return "CONST";
        case VAR: return "VAR";
        case NULL_TOKEN: return "NULL_TOKEN";
        case FN: return "FN";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case PUB: return "PUB";
        case RETURN: return "RETURN";
        case WHILE: return "WHILE";
        case VOID: return "VOID";
        case IFJ: return "IFJ";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Gets the next character from the source file.
 *
 * This function handles line numbering and EOF.
 *
 * @return int The next character, or EOF.
 */
int get_next_char() {
    int c = getchar();
    if (c == '\n') {
        line_number++;
    }
    return c;
}

/**
 * @brief Puts a character back into the source file stream.
 *
 * This function adjusts the line number if necessary.
 *
 * @param c The character to unget.
 */
void unget_char(int c) {
    if (c == '\n') {
        line_number--;
    }
    ungetc(c, stdin);
}

/**
 * @brief Checks if a lexeme is a keyword.
 *
 * @param lexeme The lexeme to check.
 * @return int 1 if lexeme is a keyword, 0 otherwise.
 */
int is_keyword(const char *lexeme) {
    const char *keywords[] = {
        "const", "var", "null", "fn", "if", "else",
        "pub", "return", "void", "while", NULL
    };
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(lexeme, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * @brief Gets the token type for a keyword lexeme.
 *
 * @param lexeme The keyword lexeme.
 * @return TOKEN_TYPE The corresponding token type.
 */
TOKEN_TYPE get_keyword_token_type(const char *lexeme) {
    if (strcmp(lexeme, "const") == 0) return CONST;
    if (strcmp(lexeme, "var") == 0) return VAR;
    if (strcmp(lexeme, "null") == 0) return NULL_TOKEN;
    if (strcmp(lexeme, "fn") == 0) return FN;
    if (strcmp(lexeme, "if") == 0) return IF;
    if (strcmp(lexeme, "else") == 0) return ELSE;
    if (strcmp(lexeme, "pub") == 0) return PUB;
    if (strcmp(lexeme, "return") == 0) return RETURN;
    if (strcmp(lexeme, "void") == 0) return VOID;
    if (strcmp(lexeme, "while") == 0) return WHILE;
    return IDENTIFIER;  // Default case (should not reach here)
}

/**
 * @brief Checks if a lexeme is a type identifier.
 *
 * @param lexeme The lexeme to check.
 * @return int 1 if lexeme is a type identifier, 0 otherwise.
 */
int is_type_identifier(const char *lexeme) {
    const char *type_identifiers[] = {
        "i32", "f64", "[]u8", NULL
    };
    for (int i = 0; type_identifiers[i] != NULL; i++) {
        if (strcmp(lexeme, type_identifiers[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
/**
 * @brief Checks if a lexeme is a nullable type identifier.
 *
 * This function compares the lexeme against a list of known nullable type identifiers.
 *
 * @param lexeme The lexeme to check.
 * @return int Returns 1 if lexeme is a nullable type identifier, 0 otherwise.
 */
int is_nullable_type_identifier(const char *lexeme) {
    const char *nullable_types[] = {
        "?i32",
        "?f64",
        "?[]u8",
        NULL  // Sentinel value to mark the end of the array
    };
    for (int i = 0; nullable_types[i] != NULL; i++) {
        if (strcmp(lexeme, nullable_types[i]) == 0) {
            return 1;  // Match found
        }
    }
    return 0;  // No match found
}

/**
 * @brief Gets the token type for a type identifier lexeme.
 *
 * @param lexeme The type identifier lexeme.
 * @return TOKEN_TYPE The corresponding token type.
 */
TOKEN_TYPE get_type_identifier_token_type(const char *lexeme) {
    if (strcmp(lexeme, "i32") == 0) return TYPE_INT;
    if (strcmp(lexeme, "f64") == 0) return TYPE_FLOAT;
    if (strcmp(lexeme, "[]u8") == 0) return TYPE_STRING;
    return IDENTIFIER;  // Default case (should not reach here)
}
/**
 * @brief Gets the token type for a nullable type identifier lexeme.
 *
 * This function maps the lexeme to the corresponding TOKEN_TYPE enum value.
 *
 * @param lexeme The nullable type identifier lexeme.
 * @return TOKEN_TYPE The corresponding token type.
 */
TOKEN_TYPE get_nullable_type_identifier_token_type(const char *lexeme) {
    if (strcmp(lexeme, "?i32") == 0) {
        return TYPE_INT_NULL;
    }
    if (strcmp(lexeme, "?f64") == 0) {
        return TYPE_FLOAT_NULL;
    }
    if (strcmp(lexeme, "?[]u8") == 0) {
        return TYPE_STRING_NULL;
    }
    // If lexeme does not match any known nullable type identifiers
    return IDENTIFIER;  // Default case (should not reach here if validation is correct)
}

/**
 * @brief The main function to get the next token from the source file.
 *
 * This function implements the FSM described and returns the next token.
 *
 * @param token Pointer to T_TOKEN structure to store the token.
 * @return int Return value indicating success or error.
 */
int get_token(T_TOKEN *token) {
    int state = 0;  // Initial state S
    int c;          // Current character
    int lexeme_size = INITIAL_LEXEME_SIZE;
    int lexeme_length = 0;
    char *lexeme = malloc(lexeme_size);
    if (lexeme == NULL) {
        // Memory allocation error
        return RET_VAL_INTERNAL_ERR;
    }
    token->line = line_number;
    token->length = 0;

    while (1) {
        c = get_next_char();
        if (c == EOF) {
            // End of file
            free(lexeme);
            // Set EOF token
            token->type = EOF_TOKEN;
            token->lexeme = NULL;
            return RET_VAL_OK;
        }

        switch (state) {
            case 0:  // State S
                if (isspace(c)) {
                    // Skip whitespace
                    continue;
                } else if (c == '.') {
                    // Transition to dot
                    token->type = DOT;
                    token->lexeme = strdup(".");
                    token->length = 1;
                    free(lexeme);
                    return RET_VAL_OK;
                } else if (c == '|') {
                    // Transition to pipe
                    token->type = PIPE;
                    token->lexeme = strdup("|");
                    token->length = 1;
                    free(lexeme);
                    return RET_VAL_OK;
                } else if (isalpha(c)) {
                    // Start of identifier or keyword
                    lexeme[lexeme_length++] = c;
                    state = 1;  // Transition to id
                } else if (c == '_') {
                    // Start of voidID
                    lexeme[lexeme_length++] = c;
                    state = 1;  // Transition to id (voidID behaves like id)
                } else if (c == '?') {
                    // Transition to question_mark
                    state = 4;  // State for handling '?'
                } else if (c == '@') {
                    // Possible start of prolog
                    lexeme[lexeme_length++] = c;
                    state = 5;  // Transition to prolog
                } else if (c == '!') {
                    // Possible start of not_equal
                    c = get_next_char();
                    if (c == '=') {
                        token->type = NOT_EQUAL;
                        token->lexeme = strdup("!=");
                        token->length = 2;
                        free(lexeme);
                        return RET_VAL_OK;
                    } else {
                        // Lexical error
                        unget_char(c);
                        free(lexeme);
                        return RET_VAL_LEXICAL_ERR;
                    }
                } else if (c == '\\') {
                    // Start of multiline string
                    state = 6;  // Transition to backslash
                } else if (c == '"') {
                    // Start of string
                    state = 7;  // Transition to quote_start
                } else if (c == '<' || c == '>' || c == '=') {
                    // Start of comparators
                    lexeme[lexeme_length++] = c;
                    state = 8;  // Transition to comparators
                } else if (isdigit(c)) {
                    // Start of number
                    lexeme[lexeme_length++] = c;
                    if (c == '0') {
                        state = 10;  // Transition to zero
                    } else {
                        state = 9;   // Transition to int_node
                    }
                } else if (c == '(' || c == ')') {
                    // Parentheses
                    token->type = (c == '(') ? BRACKET_LEFT_SIMPLE : BRACKET_RIGHT_SIMPLE;
                    lexeme[lexeme_length++] = c;
                    lexeme[lexeme_length] = '\0';
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;

                    free(lexeme);
                    return RET_VAL_OK;
                } else if (c == '{' || c == '}') {
                    // Curly braces
                    token->type = (c == '{') ? BRACKET_LEFT_CURLY : BRACKET_RIGHT_CURLY;
                    lexeme[lexeme_length++] = c;
                    lexeme[lexeme_length] = '\0';
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    free(lexeme);
                    return RET_VAL_OK;
                } else if (c == '[') {
                    // Possible start of '[]u8'
                    lexeme[lexeme_length++] = c;
                    state = 18;  // Transition to state for handling '[]u8'
                } else if (c == ';') {
                    // Semicolon
                    token->type = SEMICOLON;
                    token->lexeme = strdup(";");
                    token->length = 1;
                    free(lexeme);
                    return RET_VAL_OK;
                }
                    else if (c == ':') {
                    // Colon
                    token->type = COLON;
                    token->lexeme = strdup(":");
                    token->length = 1;
                    free(lexeme);
                    return RET_VAL_OK;
                } else if (c == '+' || c == '-' || c == '*') {
                    // Operators
                    token->type = (c == '+') ? PLUS : (c == '-') ? MINUS : MULTIPLY;
                    lexeme[lexeme_length++] = c;
                    lexeme[lexeme_length] = '\0';
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    free(lexeme);
                    return RET_VAL_OK;
                } else if (c == '/') {
                    // Possible start of comment or divide operator
                    c = get_next_char();
                    if (c == '/') {
                        // Single-line comment
                        while ((c = get_next_char()) != EOF && c != '\n');
                        state = 0;  // Return to initial state
                    } else {
                        // Divide operator
                        unget_char(c);
                        token->type = DIVIDE;
                        token->lexeme = strdup("/");
                        token->length = 1;
                        free(lexeme);
                        return RET_VAL_OK;
                    }
                } else if (c == ',') {
                    // Comma
                    token->type = COMMA;
                    token->lexeme = strdup(",");
                    token->length = 1;
                    free(lexeme);
                    return RET_VAL_OK;
                } else {
                    // Unrecognized character
                    fprintf(stderr, "Lexical error at line %d: Unrecognized character '%c'\n", line_number, c);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            case 1:  // State id
                if (isalnum(c) || c == '_') {
                    // Continue building identifier
                    lexeme[lexeme_length++] = c;
                    if (lexeme_length >= lexeme_size) {
                        lexeme_size *= 2;
                        lexeme = realloc(lexeme, lexeme_size);
                        if (lexeme == NULL) {
                            return RET_VAL_INTERNAL_ERR;
                        }
                    }
                } else {
                    // End of identifier
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    if (is_keyword(lexeme)) {
                        // Lexeme is a keyword
                        token->type = get_keyword_token_type(lexeme);
                    } else if (strcmp(lexeme, "ifj") == 0) {
                        // Lexeme is 'ifj'
                        token->type = IFJ;
                    } else if (is_type_identifier(lexeme)) {
                        // Lexeme is a type identifier
                        token->type = get_type_identifier_token_type(lexeme);
                    } else {
                        // Lexeme is an identifier
                        token->type = IDENTIFIER;
                    }
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    free(lexeme);
                    return RET_VAL_OK;
                }
                break;

            case 4:  // State question_mark
                if (c == 'i' || c == 'f' || c == '[') {
                    // Start of nullable type identifier
                    lexeme[lexeme_length++] = '?';
                    lexeme[lexeme_length++] = c;
                    state = 12;  // Transition to type_id_null
                } else {
                    // Invalid character after '?'
                    fprintf(stderr, "Lexical error at line %d: Invalid character '%c' after '?'\n", line_number, c);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            case 5:  // State prolog
                if (isalpha(c)) {
                    // Continue building prolog lexeme
                    lexeme[lexeme_length++] = c;
                    if (lexeme_length >= lexeme_size) {
                        lexeme_size *= 2;
                        lexeme = realloc(lexeme, lexeme_size);
                        if (lexeme == NULL) {
                            return RET_VAL_INTERNAL_ERR;
                        }
                    }
                } else {
                    // End of prolog lexeme
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    if (strcmp(lexeme, "@import") == 0) {
                        token->type = IMPORT;
                        token->lexeme = strdup("@import");
                        token->length = 7;
                        free(lexeme);
                        return RET_VAL_OK;
                    } else {
                        // Invalid prolog
                        fprintf(stderr, "Lexical error at line %d: Invalid prolog '%s'\n", line_number, lexeme);
                        free(lexeme);
                        return RET_VAL_LEXICAL_ERR;
                    }
                }
                break;

            case 6:  // State backslash (multiline string)
                // Implement multiline string handling based on FSM
                // ...
                break;

            case 7:  // State quote_start (string)
                if (c == '"') {
                    // End of string
                    lexeme[lexeme_length] = '\0';
                    token->type = STRING;
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    free(lexeme);
                    return RET_VAL_OK;
                } else if (c == '\\') {
                    // Escape sequence
                    lexeme[lexeme_length++] = c;
                    state = 13;  // Transition to escape sequence state
                } else if (c >= 32) {
                    // Regular character in string
                    lexeme[lexeme_length++] = c;
                    if (lexeme_length >= lexeme_size) {
                        lexeme_size *= 2;
                        lexeme = realloc(lexeme, lexeme_size);
                        if (lexeme == NULL) {
                            return RET_VAL_INTERNAL_ERR;
                        }
                    }
                } else {
                    // Invalid character in string
                    fprintf(stderr, "Lexical error at line %d: Invalid character in string\n", line_number);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            case 8:  // State comparators
                if (c == '=') {
                    // Two-character comparator
                    lexeme[lexeme_length++] = c;
                    lexeme[lexeme_length] = '\0';
                    if (strcmp(lexeme, "==") == 0) {
                        token->type = EQUAL;
                    } else if (strcmp(lexeme, "<=") == 0) {
                        token->type = LESS_THAN_EQUAL;
                    } else if (strcmp(lexeme, ">=") == 0) {
                        token->type = GREATER_THAN_EQUAL;
                    } else {
                        // Invalid comparator
                        fprintf(stderr, "Lexical error at line %d: Invalid comparator '%s'\n", line_number, lexeme);
                        free(lexeme);
                        return RET_VAL_LEXICAL_ERR;
                    }
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    free(lexeme);
                    return RET_VAL_OK;
                } else {
                    // Single-character comparator
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    if (strcmp(lexeme, "<") == 0) {
                        token->type = LESS_THAN;
                    } else if (strcmp(lexeme, ">") == 0) {
                        token->type = GREATER_THAN;
                    } else if (strcmp(lexeme, "=") == 0) {
                        token->type = ASSIGN;
                    } else {
                        // Invalid comparator
                        fprintf(stderr, "Lexical error at line %d: Invalid comparator '%s'\n", line_number, lexeme);
                        free(lexeme);
                        return RET_VAL_LEXICAL_ERR;
                    }
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    free(lexeme);
                    return RET_VAL_OK;
                }
                break;

            case 9:  // State int_node
                if (isdigit(c)) {
                    // Continue building integer
                    lexeme[lexeme_length++] = c;
                    if (lexeme_length >= lexeme_size) {
                        lexeme_size *= 2;
                        lexeme = realloc(lexeme, lexeme_size);
                        if (lexeme == NULL) {
                            return RET_VAL_INTERNAL_ERR;
                        }
                    }
                } else if (c == '.') {
                    // Transition to decimal point
                    lexeme[lexeme_length++] = c;
                    state = 11;  // Transition to float_node
                } else if (c == 'e' || c == 'E') {
                    // Transition to exponent
                    lexeme[lexeme_length++] = c;
                    state = 14;  // Transition to exponent state
                } else {
                    // End of integer
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    token->type = INT;
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    token->value.intVal = atoi(lexeme);
                    free(lexeme);
                    return RET_VAL_OK;
                }
                break;

            case 10:  // State zero
                if (c == '.') {
                    // Transition to decimal point
                    lexeme[lexeme_length++] = c;
                    state = 11;  // Transition to float_node
                } else if (c == 'e' || c == 'E') {
                    // Transition to exponent
                    lexeme[lexeme_length++] = c;
                    state = 14;  // Transition to exponent state
                } else {
                    // End of zero
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    token->type = INT;
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    token->value.intVal = 0;
                    free(lexeme);
                    return RET_VAL_OK;
                }
                break;

            case 11:  // State float_node
                if (isdigit(c)) {
                    // Continue building float
                    lexeme[lexeme_length++] = c;
                    if (lexeme_length >= lexeme_size) {
                        lexeme_size *= 2;
                        lexeme = realloc(lexeme, lexeme_size);
                        if (lexeme == NULL) {
                            return RET_VAL_INTERNAL_ERR;
                        }
                    }
                } else if (c == 'e' || c == 'E') {
                    // Transition to exponent
                    lexeme[lexeme_length++] = c;
                    state = 14;  // Transition to exponent state
                } else {
                    // End of float
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    token->type = FLOAT;
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    token->value.floatVal = atof(lexeme);
                    free(lexeme);
                    return RET_VAL_OK;
                }
                break;

            case 12:  // State type_id_null
                if (isalnum(c) || c == '[' || c == ']') {
                    // Continue building nullable type identifier
                    lexeme[lexeme_length++] = c;
                    if (lexeme_length >= lexeme_size) {
                        lexeme_size *= 2;
                        lexeme = realloc(lexeme, lexeme_size);
                        if (lexeme == NULL) {
                            return RET_VAL_INTERNAL_ERR;
                        }
                    }
                } else {
                    // End of nullable type identifier
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    if (is_nullable_type_identifier(lexeme)) {
                        token->type = get_nullable_type_identifier_token_type(lexeme);
                        token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                        free(lexeme);
                        return RET_VAL_OK;
                    } else {
                        // Invalid nullable type identifier
                        fprintf(stderr, "Lexical error at line %d: Invalid nullable type identifier '%s'\n", line_number, lexeme);
                        free(lexeme);
                        return RET_VAL_LEXICAL_ERR;
                    }
                }
                break;

            case 13:  // State escape sequence in string
                if (c == 'n' || c == 'r' || c == 't' || c == '\\' || c == '"' || c == '\'') {
                    // Valid escape sequence
                    lexeme[lexeme_length++] = c;
                    state = 7;  // Return to string state
                } else if (c == 'x') {
                    // Hexadecimal escape sequence
                    lexeme[lexeme_length++] = c;
                    state = 16;  // Transition to hex escape sequence
                } else {
                    // Invalid escape sequence
                    fprintf(stderr, "Lexical error at line %d: Invalid escape sequence '\\%c'\n", line_number, c);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            case 14:  // State exponent
                if (c == '+' || c == '-') {
                    // Exponent sign
                    lexeme[lexeme_length++] = c;
                    state = 15;  // Transition to exponent digits
                } else if (isdigit(c)) {
                    // Exponent digit
                    lexeme[lexeme_length++] = c;
                    state = 15;  // Transition to exponent digits
                } else {
                    // Invalid exponent
                    fprintf(stderr, "Lexical error at line %d: Invalid exponent in number\n", line_number);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            case 15:  // State exponent digits
                if (isdigit(c)) {
                    // Continue exponent digits
                    lexeme[lexeme_length++] = c;
                    if (lexeme_length >= lexeme_size) {
                        lexeme_size *= 2;
                        lexeme = realloc(lexeme, lexeme_size);
                        if (lexeme == NULL) {
                            return RET_VAL_INTERNAL_ERR;
                        }
                    }
                } else {
                    // End of float with exponent
                    unget_char(c);
                    lexeme[lexeme_length] = '\0';
                    token->type = FLOAT;
                    token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                    token->value.floatVal = atof(lexeme);
                    free(lexeme);
                    return RET_VAL_OK;
                }
                break;

            case 16:  // State hex escape sequence
                if (isxdigit(c)) {
                    // First hex digit
                    lexeme[lexeme_length++] = c;
                    state = 17;
                } else {
                    // Invalid hex escape sequence
                    fprintf(stderr, "Lexical error at line %d: Invalid hex escape sequence '\\x%c'\n", line_number, c);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            case 17:  // State hex escape sequence second digit
                if (isxdigit(c)) {
                    // Second hex digit
                    lexeme[lexeme_length++] = c;
                    state = 7;  // Return to string state
                } else {
                    // Invalid hex escape sequence
                    fprintf(stderr, "Lexical error at line %d: Invalid hex escape sequence '\\x%c'\n", line_number, c);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;
            case 18:  // State handling '[]u8'
                if (c == ']') {
                    lexeme[lexeme_length++] = c;
                    c = get_next_char();
                    if (c == 'u') {
                        lexeme[lexeme_length++] = c;
                        c = get_next_char();
                        if (c == '8') {
                            lexeme[lexeme_length++] = c;
                            lexeme[lexeme_length] = '\0';
                            token->type = TYPE_STRING;
                            token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                            free(lexeme);
                            return RET_VAL_OK;
                        } else {
                            // Lexical error
                            unget_char(c);
                            fprintf(stderr, "Lexical error at line %d: Expected '8' after 'u'\n", line_number);
                            free(lexeme);
                            return RET_VAL_LEXICAL_ERR;
                        }
                    } else {
                        // Lexical error
                        unget_char(c);
                        fprintf(stderr, "Lexical error at line %d: Expected 'u' after ']'\n", line_number);
                        free(lexeme);
                        return RET_VAL_LEXICAL_ERR;
                    }
                } else {
                    // Lexical error
                    unget_char(c);
                    fprintf(stderr, "Lexical error at line %d: Expected ']' after '[', got %c\n", line_number, c);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            case 19:  // State handling '?[]u8'
                if (c == ']') {
                    lexeme[lexeme_length++] = c;
                    c = get_next_char();
                    if (c == 'u') {
                        lexeme[lexeme_length++] = c;
                        c = get_next_char();
                        if (c == '8') {
                            lexeme[lexeme_length++] = c;
                            lexeme[lexeme_length] = '\0';
                            token->type = TYPE_STRING_NULL;
                            token->lexeme = strdup(lexeme);
                            token->length = lexeme_length;
                            free(lexeme);
                            return RET_VAL_OK;
                        } else {
                            // Lexical error
                            unget_char(c);
                            fprintf(stderr, "Lexical error at line %d: Expected '8' after 'u' in nullable string type\n", line_number);
                            free(lexeme);
                            return RET_VAL_LEXICAL_ERR;
                        }
                    } else {
                        // Lexical error
                        unget_char(c);
                        fprintf(stderr, "Lexical error at line %d: Expected 'u' after ']'\n", line_number);
                        free(lexeme);
                        return RET_VAL_LEXICAL_ERR;
                    }
                } else {
                    // Lexical error
                    unget_char(c);
                    fprintf(stderr, "Lexical error at line %d: Expected ']' after '[' in nullable string type\n", line_number);
                    free(lexeme);
                    return RET_VAL_LEXICAL_ERR;
                }
                break;

            default:
                // Invalid state
                fprintf(stderr, "Internal error: Invalid scanner state\n");
                free(lexeme);
                return RET_VAL_INTERNAL_ERR;
        }
    }
    // Should not reach here
    free(lexeme);
    return RET_VAL_INTERNAL_ERR;
}
