// FILE: symtable.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Marek Tenora> (xtenor02)
// <Otakar Kočí> (xkocio00)
// Martin Zůbek (253206)
// Kryštof Valenta (xvalenk00)
//
// YEAR: 2024
// NOTES:   Implementation of Symtable with hash table and stack of scopes.
//          Also contains helper functions for easier manipulation with the symtable
//          from other parts of the compiler.

#include "symtable.h"
#include "semantic.h"

// Initialize hashtable
Hashtable *hashtable_init() {
    Hashtable *ht = (Hashtable *)malloc(sizeof(Hashtable));
    if (!ht) return NULL;
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        ht->table[i].occupied = false; // Mark all slots as unoccupied initially
        ht->table[i].deleted = false;
        ht->table[i].name = NULL;
        ht->table[i].type = SYM_VAR;
        ht->table[i].data.var.is_const = false;
        ht->table[i].data.var.type = VAR_VOID;
    }
    ht->count = 0;
    return ht;
}

// Free hashtable
void hashtable_free(Hashtable *ht) {
    if (!ht) return;
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        if (ht->table[i].occupied) { // Only free if slot is occupied
            free(ht->table[i].name);
            if (ht->table[i].type == SYM_FUNC) {
                free(ht->table[i].data.func.argv);
            }
        }
    }
    free(ht);
}

// Primary hash function
unsigned int hash_function(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 31) + *key++;
    }
    return hash % HASHTABLE_SIZE;
}

// Secondary hash function (for double hashing)
unsigned int secondary_hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash * 17) + *key++;
    }
    return (hash % (HASHTABLE_SIZE - 2)) + 1;
}

// Insert into hashtable using open addressing with Brent's optimization
Symbol *hashtable_insert(Hashtable *ht, const char *key, SymbolType type, SymbolData data) {
    if (!ht || ht->count >= HASHTABLE_SIZE) return NULL;

    unsigned int index = hash_function(key);
    unsigned int step = secondary_hash(key);
    int probe_count = 0;

    Symbol new_symbol = { .name = strdup(key), .type = type, .data = data, .occupied = true };

    while (ht->table[index].occupied) {
        if (strcmp(ht->table[index].name, key) == 0) {
            free(new_symbol.name);  // Avoid duplicate insertion
            return NULL;
        }
        if (ht->table[index].deleted) {
            // Reuse a deleted slot
            ht->table[index].deleted = false;
            ht->table[index].occupied = true;
        }

        // Implement Brent's Optimization: Calculate an alternative position
        int alt_index = (index + step) % HASHTABLE_SIZE;
        if (!ht->table[alt_index].occupied || probe_count + 1 < alt_index) {
            // Swap the entries to minimize probe length
            Symbol temp = ht->table[index];
            ht->table[index] = new_symbol;
            new_symbol = temp;
            index = alt_index;
        } else {
            // Move to the next position in probe sequence
            index = (index + step) % HASHTABLE_SIZE;
        }
        probe_count++;
    }

    // Place the new symbol in the resolved index
    ht->table[index] = new_symbol;
    ht->count++;
    return &ht->table[index];
}

// Find an entry in the hashtable
Symbol *hashtable_find(Hashtable *ht, const char *key) {
    if (!ht) return NULL;

    unsigned int index = hash_function(key);
    unsigned int step = secondary_hash(key);
    int probe_count = 0;

    while (ht->table[index].occupied || ht->table[index].deleted) { // Only continue if the slot is occupied or was deleted
        if (ht->table[index].occupied && strcmp(ht->table[index].name, key) == 0) {
            //ht->table[index].data.var.used = true;
            return &ht->table[index];
        }
        if (probe_count++ >= HASHTABLE_SIZE) break; // Avoid infinite loop if table is full
        index = (index + step) % HASHTABLE_SIZE;
    }

    return NULL; // Key not found
}

// Remove an entry from the hashtable
void hashtable_remove(Hashtable *ht, const char *key) {
    if (!ht) return;

    unsigned int index = hash_function(key);
    unsigned int step = secondary_hash(key);
    int probe_count = 0;

    while (ht->table[index].occupied) {
        if (strcmp(ht->table[index].name, key) == 0) {
            // Free dynamically allocated fields
            free(ht->table[index].name);
            if (ht->table[index].type == SYM_FUNC) {
                free(ht->table[index].data.func.argv);
            }

            // Mark slot as unoccupied
            ht->table[index].occupied = false;
            ht->table[index].deleted = true;
            ht->count--;
            return;
        }
        if (probe_count++ >= HASHTABLE_SIZE) break;
        index = (index + step) % HASHTABLE_SIZE;
    }
}

// Initialize symbol table
T_SYM_TABLE *symtable_init()
{
    T_SYM_TABLE *table = (T_SYM_TABLE*) malloc(sizeof(T_SYM_TABLE));
    if (table == NULL)
    {
        return NULL;
    }
    table->var_id_cnt = 0;
    table->label_cnt = 0;
    table->while_def_cnt = 0;
    table->top = NULL;
    return table;
}

// Add a new scope to the symbol table
bool symtable_add_scope(T_SYM_TABLE *table, bool is_while) {
    if (table == NULL) {
        return false;
    }
    T_SCOPE *new_scope = (T_SCOPE *) malloc(sizeof(T_SCOPE));
    if (new_scope == NULL) {
        return false;
    }
    new_scope->ht = hashtable_init();
    if (new_scope->ht == NULL) {
        free(new_scope);
        return false;
    }
    if (is_while) {
        new_scope->while_defined_id = table->while_def_cnt++;
    } else {
        new_scope->while_defined_id = -1;
    }
    new_scope->parent = table->top;
    table->top = new_scope;
    return true;
}

// Remove the top scope from the symbol table
int symtable_remove_scope(T_SYM_TABLE *table, bool check_unused_vars) {
    if (table == NULL || table->top == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }
    
    // Check for unused and unmodified variables
    if (check_unused_vars) {
        int error_code = check_for_unused_vars(table);
        if (error_code != RET_VAL_OK) {
            return error_code;
        }
    }

    T_SCOPE *old_scope = table->top;
    table->top = old_scope->parent;
    hashtable_free(old_scope->ht);
    free(old_scope);
    return RET_VAL_OK;
}

// Add a new symbol to the current scope
Symbol *symtable_add_symbol(T_SYM_TABLE *table, const char *key, SymbolType type, SymbolData data) {
    if (table == NULL || table->top == NULL) {
        return NULL;
    }
    if (type == SYM_VAR)
        data.var.id = table->var_id_cnt++;
    return hashtable_insert(table->top->ht, key, type, data);
}

// Find a symbol in the symbol table,  go through all scopes
Symbol *symtable_find_symbol(T_SYM_TABLE *table, const char *key) {
    if (table == NULL || table->top == NULL) {
        return NULL;
    }
    
    T_SCOPE *current_scope = table->top;
    while (current_scope != NULL) {
        Symbol *symbol = hashtable_find(current_scope->ht, key);
        if (symbol != NULL) {
            return symbol;
        }
        current_scope = current_scope->parent;
    }

    return NULL;
}

// Free the symbol table
void symtable_free(T_SYM_TABLE *table) {
    if (table == NULL) {
        return;
    }
    while (table->top != NULL) {
        symtable_remove_scope(table, false);
    }
    free(table);
}

// Add a parameter to the symbol data
int add_param_to_symbol_data(SymbolData *data, Param param) {
    if (data == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }
    if (data->func.argc == 0) {
        data->func.argv = (Param *) malloc(sizeof(Param));
        if (data->func.argv == NULL) {
            return RET_VAL_INTERNAL_ERR;
        }
    } else {
        Param *new_argv = (Param *) realloc(data->func.argv, (data->func.argc + 1) * sizeof(Param));
        if (new_argv == NULL) {
            return RET_VAL_INTERNAL_ERR;
        }
        data->func.argv = new_argv;
    }
    data->func.argv[data->func.argc++] = param;
    return RET_VAL_OK;
}

// Get the var id
int get_var_id(T_SYM_TABLE *table, const char *key) {
    Symbol *symbol = symtable_find_symbol(table, key);
    if (symbol == NULL) {
        return -1;
    }
    return symbol->data.var.id;
}

// get variable
Symbol *get_var(T_SYM_TABLE *table, const char *name) {
    Symbol *symbol = symtable_find_symbol(table, name);
    if (symbol == NULL) {
        return NULL;
    }
    if (symbol->type != SYM_VAR) {
        return NULL;
    }
    return symbol;
}

// Checks for unused and unmodified variables in the symbol table
int check_for_unused_vars(T_SYM_TABLE *table) {
    if (table == NULL || table->top == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    Hashtable *ht = table->top->ht;
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        if (ht->table[i].occupied && ht->table[i].type == SYM_VAR) {
            if (!ht->table[i].data.var.used || !ht->table[i].data.var.modified) {
                //fprintf(stderr, "Variable '%s' was never used", ht->table[i].name);
                return RET_VAL_SEMANTIC_UNUSED_VAR_ERR;
            }
        }
    }

    return RET_VAL_OK;
}

// TODO: finish

bool generate_labels(T_SYM_TABLE *table, char **label1, char **label2) {
    if (table == NULL || table->top == NULL) {
        return false;
    }

    int label_cnt_1 = table->label_cnt++;
    int label_cnt_2 = table->label_cnt++;
    
    size_t len = snprintf(NULL, 0, "$%d", label_cnt_1);
    (*label1) = (char *) malloc(len + 1);
    if ((*label1) == NULL) {
        return false;
    }
    sprintf((*label1), "$%d", label_cnt_1);

    len = snprintf(NULL, 0, "$%d", label_cnt_2);
    (*label2) = (char *) malloc(len + 1);
    if ((*label2) == NULL) {
        free(*label1);
        return false;
    }
    sprintf((*label2), "$%d", label_cnt_2);

    return true;
}

// Returns the id of the while loop that is closest to the top of the stack (current scope)
// or -1 if there is none
int is_in_while(T_SYM_TABLE *table) {
    if (table == NULL || table->top == NULL) {
        return -1;
    }
    T_SCOPE *current_scope = table->top;
    while (current_scope != NULL) {
        if (current_scope->while_defined_id != -1) {
            return current_scope->while_defined_id;
        }
        current_scope = current_scope->parent;
    }
    return -1;
}