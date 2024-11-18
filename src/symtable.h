#ifndef H_SYMTABLE
#define H_SYMTABLE

#include <stdbool.h>
#include "shared.h"
#include "scanner.h"

// Variable types
typedef enum {
    VAR_INT,
    VAR_FLOAT,
    VAR_STRING,
    VAR_INT_NULL,
    VAR_FLOAT_NULL,
    VAR_STRING_NULL
} VarType;

// Parameter structure
typedef struct {
    char *name;
    VarType type;
} Param;

// Symbol types
typedef enum {
    SYM_VAR,
    SYM_FUNC
} SymbolType;

typedef union
{
    struct {
        bool is_const;
        VarType type;
        void *value;
    } var;
    struct {
        VarType return_type;
        int argc;
        Param *argv;
    } func;
} SymbolData;


// Symbol structure
typedef struct Symbol {
    char *name;
    SymbolType type;
    SymbolData data;
    bool used;
    struct Symbol *next; // For handling collisions in the hashtable
} Symbol;

#endif // H_SYMTABLE