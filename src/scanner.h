//FILE: scanner.h
//PROJEKT: IFJ24 - Compiler for the IFJ24 lang @ FIT BUT 2BIT
//TEAM: Martin Zůbek (253206)
//AUTHORS:
// Otakar Kočí (xkocio00, 247555)
//
//YEAR: 2024
//NOTES: 

#ifndef H_SCANNER
#define H_SCANNER

// TYPE DEFINITIONS

typedef enum TOKEN_TYPE{
    IMPORT,
    NOT_EQUAL,
    STRING, // MULTILINE_STRING goes here as well
    INT,
    FLOAT,
    LESS_THAN,
    GREATER_THAN,
    ASSIGN,
    EQUAL,
    LESS_THAN_EQUAL,
    GREATER_THAN_EQUAL,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    COLON,
    SEMICOLON,
    BRACKET_LEFT_SIMPLE,
    BRACKET_RIGHT_SIMPLE,
    BRACKET_LEFT_CURLY,
    BRACKET_RIGHT_CURLY,
    IDENTIFIER,
    IDENTIFIER_DISCARD,
    TYPE_INT, // I32
    TYPE_FLOAT, // F64
    TYPE_STRING, // []U8
    TYPE_INT_NULL, // ?I32
    TYPE_FLOAT_NULL, // ?F64
    TYPE_STRING_NULL, // ?[]U8
    DOT,
    COMMA,
    PIPE, // |
    CONST,
    VAR,
    NULL_TOKEN,
    FN,
    IF,
    ELSE,
    PUB,
    RETURN,
    WHILE,
    VOID,
    IFJ,
    EOF_TOKEN,
    VOID_TOKEN // Special token used in token buffer to return when no token is found
} TOKEN_TYPE;

/**
 * @brief Structure representing a token.
 */
typedef struct T_TOKEN {
    TOKEN_TYPE type;     // The type of the token
    char *lexeme;        // The lexeme (string representation)
    int line;            // Line number where the token was found
    int length;          // Length of the lexeme
    union {
        int int_val;      // Integer value for INT tokens
        float float_val;  // Float value for FLOAT tokens
        char *str_val; // String value for STRING tokens
    } value;             // Union to hold the token's value
} T_TOKEN;

/**
 * @brief Gets the next token from the source file.
 *
 * This function implements the FSM described and returns the next token.
 *
 * @param token Pointer to T_TOKEN structure to store the token.
 * @return int Return value indicating success or error.
 */
int get_token(T_TOKEN *token);

/**
 * @brief Gets the string representation of a token type.
 *
 * @param type The token type.
 * @return const char* The string representation.
 */
const char* tt_to_str(TOKEN_TYPE type);


#endif // H_SCANNER