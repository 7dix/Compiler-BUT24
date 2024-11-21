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
#include <stdlib.h>
#include <string.h>



// Variable types
typedef enum {
    VAR_INT,
    VAR_FLOAT,
    VAR_STRING,
    VAR_INT_NULL,
    VAR_FLOAT_NULL,
    VAR_STRING_NULL,
    STRING_VAR_STRING, // special case for ifj.string()
    VAR_ANY, // special case for ifj.write()
    VAR_VOID
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
        bool modified;
        bool used;
        VarType type;
        void *value;
    } var;
    struct {
        VarType return_type;
        int argc;
        Param *argv;
    } func;
} SymbolData;

int add_param_to_symbol_data(SymbolData *data, Param param);


// Symbol structure
typedef struct Symbol {
    char *name;
    SymbolType type;
    SymbolData data;
    struct Symbol *next; // For handling collisions in the hashtable
    bool occupied; // Marks if this slot is occupied
    bool deleted; // Marks if this slot was deleted
} Symbol;

//-----------------------------------HASH TABLE-----------------------------------//

#define HASHTABLE_SIZE 1999

// Hashtable structure
typedef struct {
    Symbol table[HASHTABLE_SIZE]; // Array of entry pointers
    int count; // Tracks number of entries
} Hashtable;

// Hashtable functions
Hashtable *hashtable_init();
void hashtable_free(Hashtable *ht);
Symbol *hashtable_insert(Hashtable *ht, const char *key, SymbolType type, SymbolData data);
Symbol *hashtable_find(Hashtable *ht, const char *key);
void hashtable_remove(Hashtable *ht, const char *key);



//-----------------------------------SYMBOL TABLE-----------------------------------//

typedef struct T_SCOPE {
    Hashtable *ht;
    unsigned int label_cnt_1;
    unsigned int label_cnt_2;
    struct T_SCOPE *parent;
} T_SCOPE;

typedef struct T_SYM_TABLE {
    T_SCOPE *top;
    unsigned int label_cnt;
} T_SYM_TABLE;

// Symbol table functions
T_SYM_TABLE *symtable_init();
bool symtable_add_scope(T_SYM_TABLE *table);
void symtable_remove_scope(T_SYM_TABLE *table);
Symbol *symtable_add_symbol(T_SYM_TABLE *table, const char *key, SymbolType type, SymbolData data);
Symbol *symtable_find_symbol(T_SYM_TABLE *table, const char *key);
void symtable_free(T_SYM_TABLE *table);

extern T_SYM_TABLE *ST;

#endif // H_SYMTABLE