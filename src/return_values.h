//FILE: return_values.h
//PROJEKT: IFJ24 - Compiler for the IFJ24 lang @ FIT BUT 2BIT
//TEAM: Martin Zůbek (253206)
//AUTHORS:
// Otakar Kočí (xkocio00, 247555)
//
//YEAR: 2024
//NOTES: Return values definitions for the IFJ24 language compiler


#ifndef H_DEFINITIONS
#define H_DEFINITIONS

// RETURN VALUES DEFINITIONS
typedef enum {
    RET_VAL_OK = 0,
    RET_VAL_LEXICAL_ERR = 1,
    // Wrong syntax, missing header
    RET_VAL_SYNTAX_ERR = 2,
    // Undefined function or variable
    RET_VAL_SEMANTIC_UNDEFINED_ERR = 3,
    // Function call (wrong type or number of params), return value (wrong type or not allowed discard)
    RET_VAL_SEMANTIC_FUNCTION_ERR = 4,
    // Redefinition of variable or function, assignment to constant
    RET_VAL_SEMANTIC_REDEF_OR_BAD_ASSIGN_ERR = 5,
    // Missing or too many expressions in function return
    RET_VAL_SEMANTIC_FUNC_RETURN_ERR = 6,
    // Wrong type compat in arithmetic, string and/or relation expr., incompatible type during assignment
    RET_VAL_SEMANTIC_TYPE_COMPATIBILITY_ERR = 7,
    // Type is not stated and cannot be derived from the expression
    RET_VAL_SEMANTIC_TYPE_DERIVATION_ERR = 8,
    // Unused variable, modifiable var without possibility of change after init
    RET_VAL_SEMANTIC_UNUSED_VAR_ERR = 9,
    RET_VAL_SEMANTIC_OTHER_ERR = 10,
    // Internal errors of the compiler -> insufficient memory, etc.
    RET_VAL_INTERNAL_ERR = 99
} T_RET_VAL;



#endif // H_DEFINITIONS