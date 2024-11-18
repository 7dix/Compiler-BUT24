// FILE: symtable.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Marek Tenora> (xtenor02)
// <Otakar Kočí> (xkocio00)
//
// YEAR: 2024

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

//-----------------------------------HASH TABLE-----------------------------------//

#define HASHTABLE_SIZE 2000

// Hashtable structure
typedef struct {
    Symbol *table[HASHTABLE_SIZE]; // Array of entry pointers
    int count; // Tracks number of entries
} Hashtable;

// Hashtable functions
Hashtable *hashtable_init();
void hashtable_free(Hashtable *ht);
Symbol *hashtable_insert(Hashtable *ht, const char *key, SymbolType type, SymbolData data);
Symbol *hashtable_find(Hashtable *ht, const char *key);
void hashtable_remove(Hashtable *ht, const char *key);


#endif // H_SYMTABLE