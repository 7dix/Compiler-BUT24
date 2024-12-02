// FILE: symtable.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Marek Tenora> (xtenor02)
//  <Otakar Kočí> (xkocio00)
//  <Martin Zůbek> (253206)
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES:   Header file for symtable. Contains structure declarations and function prototypes.

#ifndef H_SYMTABLE
#define H_SYMTABLE

#include <stdbool.h>
#include "return_values.h"
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
    VAR_NULL, // not used in vars, unification with expr checks
    VAR_BOOL, // not used in vars, unification with expr checks
    STRING_LITERAL,
    VAR_VOID,
    VAR_NONE,
} VAR_TYPE;

// Parameter structure
typedef struct {
    char *name;
    VAR_TYPE type;
} T_PARAM;

// T_SYMBOL types
typedef enum {
    SYM_VAR,
    SYM_FUNC
} SYMBOL_TYPE;

typedef union
{
    struct {
        bool is_const;
        bool modified;
        bool used;
        bool const_expr;
        float float_value;
        VAR_TYPE type;
        int id;
    } var;
    struct {
        VAR_TYPE return_type;
        int argc;
        T_PARAM *argv;
    } func;
} T_SYMBOL_DATA;

int add_param_to_symbol_data(T_SYMBOL_DATA *data, T_PARAM param);


// T_SYMBOL structure
typedef struct T_SYMBOL {
    char *name;
    SYMBOL_TYPE type;
    T_SYMBOL_DATA data;
    struct T_SYMBOL *next; // For handling collisions in the hashtable
    bool occupied; // Marks if this slot is occupied
    bool deleted; // Marks if this slot was deleted
} T_SYMBOL;

//-----------------------------------HASH TABLE-----------------------------------//

#define HASHTABLE_SIZE 1999

// T_HASHTABLE structure
typedef struct {
    T_SYMBOL table[HASHTABLE_SIZE]; // Array of entry pointers
    int count; // Tracks number of entries
} T_HASHTABLE;

// T_HASHTABLE functions
T_HASHTABLE *hashtable_init();
void hashtable_free(T_HASHTABLE *ht);
T_SYMBOL *hashtable_insert(T_HASHTABLE *ht, const char *key, SYMBOL_TYPE type, T_SYMBOL_DATA data);
T_SYMBOL *hashtable_find(T_HASHTABLE *ht, const char *key);
void hashtable_remove(T_HASHTABLE *ht, const char *key);



//-----------------------------------SYMBOL TABLE-----------------------------------//

// Scope structure for symtable stack
typedef struct T_SCOPE {
    T_HASHTABLE *ht;
    struct T_SCOPE *parent;
    int fc_defined_id;
} T_SCOPE;

// Symtable encapsulation structure
typedef struct T_SYM_TABLE {
    T_SCOPE *top;
    int label_cnt;
    int var_id_cnt;
    int fc_defined_cnt;
    char *current_fn_name;
} T_SYM_TABLE;


// Symtable main functions

T_SYM_TABLE *symtable_init();
bool symtable_add_scope(T_SYM_TABLE *table, bool is_while);
int symtable_remove_scope(T_SYM_TABLE *table, bool check_unused_vars);
T_SYMBOL *symtable_add_symbol(T_SYM_TABLE *table, const char *key, SYMBOL_TYPE type, T_SYMBOL_DATA data);
T_SYMBOL *symtable_find_symbol(T_SYM_TABLE *table, const char *key);
void symtable_free(T_SYM_TABLE *table);


// Secondary symtable functions

int get_var_id(T_SYM_TABLE *table, const char *key);
T_SYMBOL *get_var(T_SYM_TABLE *table, const char *name);
int check_for_unused_vars(T_SYM_TABLE *table);
bool generate_labels(T_SYM_TABLE *table, char **label1, char **label2);
int is_in_fc(T_SYM_TABLE *table);
void set_fn_name(T_SYM_TABLE *table, char *name);
char *get_fn_name(T_SYM_TABLE *table);


extern T_SYM_TABLE *ST;

#endif // H_SYMTABLE