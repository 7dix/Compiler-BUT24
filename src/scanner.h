//FILE: scanner.h
//PROJEKT: IFJ24 - Compiler for the IFJ24 lang @ FIT BUT 2BIT
//TEAM: Martin Zůbek (253226)
//AUTHORS:
// Otakar Kočí (xkocio00, 247555)
//
//YEAR: 2024
//NOTES: 

#ifndef H_SCANNER
#define H_SCANNER

// TYPE DEFINITIONS

typedef enum TOKEN_TYPE{
    /*
    trochu detailneji jsem prepsal typy tokenu podle automatu,
    dal by to chtelo jeste probrat (aspon call) 
    */
    IMPORT,
    NOT_EQUAL,
    STRING, // MULTILINE_STRING by spadal pod string
    INTEGER,
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
    SEMICOLON, // EOL
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
    NULL,
    FN,
    IF,
    ELSE,
    PUB,
    RETURN,
    WHILE,
    VOID,
    IFJ,
} TOKEN_TYPE;

typedef struct T_TOKEN {
    TOKEN_TYPE type;
    char *lexeme;
    int line;
    int length;
    union {
        int intVal;
        float floatVal;
        char *stringVal; // change?
    } value;
} T_TOKEN;

//

#endif // H_SCANNER