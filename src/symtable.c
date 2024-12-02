// FILE: symtable.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Marek Tenora> (xtenor02)
//  <Otakar Kočí> (xkocio00)
//  <Martin Zůbek> (253206)
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES:   Implementation of Symtable with hash table and stack of scopes.
//          Also contains helper functions for easier manipulation with the symtable
//          from other parts of the compiler.

#include "symtable.h"
#include "semantic.h"

// Initialize hashtable
T_HASHTABLE *hashtable_init() {
    T_HASHTABLE *ht = (T_HASHTABLE *)malloc(sizeof(T_HASHTABLE));
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
void hashtable_free(T_HASHTABLE *ht) {
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
T_SYMBOL *hashtable_insert(T_HASHTABLE *ht, const char *key, SYMBOL_TYPE type, T_SYMBOL_DATA data) {
    if (!ht || ht->count >= HASHTABLE_SIZE) return NULL;

    unsigned int index = hash_function(key);
    unsigned int step = secondary_hash(key);
    int probe_count = 0;

    T_SYMBOL new_symbol = { .name = strdup(key), .type = type, .data = data, .occupied = true };

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
            T_SYMBOL temp = ht->table[index];
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
T_SYMBOL *hashtable_find(T_HASHTABLE *ht, const char *key) {
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
void hashtable_remove(T_HASHTABLE *ht, const char *key) {
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

/**
 * @brief Initialize the symbol table
 * 
 * @return T_SYM_TABLE* Pointer to the symbol table, or null if allocation failed
 */
T_SYM_TABLE *symtable_init()
{
    T_SYM_TABLE *table = (T_SYM_TABLE*) malloc(sizeof(T_SYM_TABLE));
    if (table == NULL)
        return NULL;

    table->var_id_cnt = 0;
    table->label_cnt = 0;
    table->fc_defined_cnt = 0;
    table->current_fn_name = NULL;
    table->top = NULL;
    return table;
}

/**
 * @brief Add a new scope to the symbol table
 * 
 * Also sets the flow control id if the scope is a while/if/else
 * 
 * @param table Pointer to the symbol table
 * @param is_fc True if the scope is a flow control statement
 * @return true added successfully
 * @return false if operation failed
 */
bool symtable_add_scope(T_SYM_TABLE *table, bool is_fc) {
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
    // if the scope is a flow control statement, set the id
    // for handling of DEFVARs (encapsulated in flow controls) in codegen
    if (is_fc) {
        new_scope->fc_defined_id = table->fc_defined_cnt++;
    } else {
        new_scope->fc_defined_id = -1;
    }

    new_scope->parent = table->top;
    table->top = new_scope;
    return true;
}

/**
 * @brief Remove the top scope from the symbol table
 * 
 * @param table Pointer to the symbol table
 * @param check_unused_vars if true, check for unused and unmodified variables
 * @return int RET_VAL_OK if operation was successful
 * @return int RET_VAL_INTERNAL_ERR if operation failed
 * @return int RET_VAL_SEMANTIC_UNUSED_VAR_ERR if unused variable was found
 */
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

/**
 * @brief Add a symbol to the symbol table
 * 
 * @param table Pointer to the symbol table
 * @param key Symbols name
 * @param type Symbols type
 * @param data Symbols data
 * @return T_SYMBOL* Pointer to the symbol, or null if operation failed
 */
T_SYMBOL *symtable_add_symbol(T_SYM_TABLE *table, const char *key, SYMBOL_TYPE type, T_SYMBOL_DATA data) {
    if (table == NULL || table->top == NULL) {
        return NULL;
    }

    if (type == SYM_VAR) {
        data.var.id = table->var_id_cnt++;
    }
    return hashtable_insert(table->top->ht, key, type, data);
}

/**
 * @brief Find a symbol in the symbol table
 * 
 * @param table Pointer to the symbol table
 * @param key Symbols name
 * @return T_SYMBOL* Pointer to the symbol, or null if not found
 */
T_SYMBOL *symtable_find_symbol(T_SYM_TABLE *table, const char *key) {
    if (table == NULL || table->top == NULL) {
        return NULL;
    }
    
    T_SCOPE *current_scope = table->top;
    while (current_scope != NULL) {
        T_SYMBOL *symbol = hashtable_find(current_scope->ht, key);
        if (symbol != NULL) {
            return symbol;
        }
        current_scope = current_scope->parent;
    }

    return NULL;
}

/**
 * @brief Free the symbol table and all its scopes
 * 
 * @param table Pointer to the symbol table
 */
void symtable_free(T_SYM_TABLE *table) {
    if (table == NULL) {
        return;
    }
    while (table->top != NULL) {
        // Remove all scopes, do not check for unused variables
        symtable_remove_scope(table, false);
    }
    free(table);
}

/**
 * @brief Add a parameter to the symbol data
 * 
 * Adds a parameter to the symbol data of a function, reallocates the argv array
 * 
 * 
 * @param data Pointer to the symbol data
 * @param param Parameter to add
 * @return int RET_VAL_OK if operation was successful
 * @return int RET_VAL_INTERNAL_ERR if operation failed
 */
int add_param_to_symbol_data(T_SYMBOL_DATA *data, T_PARAM param) {
    if (data == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }
    if (data->func.argc == 0) {
        data->func.argv = (T_PARAM *) malloc(sizeof(T_PARAM));
        if (data->func.argv == NULL) {
            return RET_VAL_INTERNAL_ERR;
        }
    } else {
        T_PARAM *new_argv = (T_PARAM *) realloc(data->func.argv, (data->func.argc + 1) * sizeof(T_PARAM));
        if (new_argv == NULL) {
            return RET_VAL_INTERNAL_ERR;
        }
        data->func.argv = new_argv;
    }
    data->func.argv[data->func.argc++] = param;
    return RET_VAL_OK;
}

/**
 * @brief Get the id of a variable
 * 
 * @param table Pointer to the symbol table
 * @param key Variable name
 * @return int Variable id, or -1 if not found
 */
int get_var_id(T_SYM_TABLE *table, const char *key) {
    T_SYMBOL *symbol = symtable_find_symbol(table, key);
    if (symbol == NULL) {
        return -1;
    }
    return symbol->data.var.id;
}

/**
 * @brief Get a variable from the symbol table
 * 
 * @param table Pointer to the symbol table
 * @param key Variable name
 * @return T_SYMBOL* Pointer to the symbol, or null if not found or not a variable (fn)
 */
T_SYMBOL *get_var(T_SYM_TABLE *table, const char *name) {
    T_SYMBOL *symbol = symtable_find_symbol(table, name);
    if (symbol == NULL) {
        return NULL;
    }
    if (symbol->type != SYM_VAR) {
        return NULL;
    }
    return symbol;
}

/**
 * @brief Check for unused variables in the symbol table
 * 
 * Checks variable usage and modification flags
 * 
 * @param table Pointer to the symbol table
 * @return int RET_VAL_OK if no unused variables were found
 * @return int RET_VAL_SEMANTIC_UNUSED_VAR_ERR if unused variable was found
 * @return int RET_VAL_INTERNAL_ERR if operation failed
 */
int check_for_unused_vars(T_SYM_TABLE *table) {
    if (table == NULL || table->top == NULL) {
        return RET_VAL_INTERNAL_ERR;
    }

    T_HASHTABLE *ht = table->top->ht;
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        if (ht->table[i].occupied && ht->table[i].type == SYM_VAR) {
            if (!ht->table[i].data.var.used || !ht->table[i].data.var.modified) {
                return RET_VAL_SEMANTIC_UNUSED_VAR_ERR;
            }
        }
    }

    return RET_VAL_OK;
}

/**
 * @brief Generate two unique labels
 * 
 * Uses the label counter from the symbol table to generate two unique labels
 * 
 * @param table Pointer to the symbol table
 * @param label1 Pointer to the first label
 * @param label2 Pointer to the second label
 * @return true if labels were generated successfully
 * @return false if operation failed
 */
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

/**
 * @brief Check if we are in a flow control statement
 * 
 * The flow control scope can be anywhere above the current scope
 * 
 * @param table Pointer to the symbol table
 * @return int Flow control id, or -1 if not in a flow control statement
 */
int is_in_fc(T_SYM_TABLE *table) {
    if (table == NULL || table->top == NULL) {
        return -1;
    }

    T_SCOPE *current_scope = table->top;

    while (current_scope != NULL) {
        if (current_scope->fc_defined_id != -1) {
            return current_scope->fc_defined_id;
        }
        current_scope = current_scope->parent;
    }
    return -1;
}

/**
 * @brief Set the current function name
 * 
 * @param table Pointer to the symbol table
 * @param name Function name
 */
void set_fn_name(T_SYM_TABLE *table, char *name) {
    if (table == NULL) {
        return;
    }
    table->current_fn_name = name;
}

/**
 * @brief Get the current function name
 * 
 * @param table Pointer to the symbol table
 * @return char* Function name
 */
char *get_fn_name(T_SYM_TABLE *table) {
    if (table == NULL) {
        return NULL;
    }
    return table->current_fn_name;
}