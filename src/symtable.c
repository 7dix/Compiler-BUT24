// FILE: symtable.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Marek Tenora> (xtenor02)
// <Otakar Kočí> (xkocio00)
//
// YEAR: 2024
#include "symtable.h"

// Initialize hashtable
Hashtable *hashtable_init() {
    Hashtable *ht = (Hashtable *)malloc(sizeof(Hashtable));
    if (!ht) return NULL;
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        ht->table[i].occupied = false; // Mark all slots as unoccupied initially
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
            } else if (ht->table[i].type == SYM_VAR) {
                free(ht->table[i].data.var.value);
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
    return (hash % (HASHTABLE_SIZE - 1)) + 1;
}

// Insert into hashtable using open addressing with Brent's optimization
Symbol *hashtable_insert(Hashtable *ht, const char *key, SymbolType type, SymbolData data) {
    if (!ht || ht->count >= HASHTABLE_SIZE) return NULL;

    unsigned int index = hash_function(key);
    unsigned int step = secondary_hash(key);
    int probe_count = 0;

    Symbol new_symbol = { .name = strdup(key), .type = type, .data = data, .used = false, .occupied = true };

    while (ht->table[index].occupied) {
        if (strcmp(ht->table[index].name, key) == 0) {
            free(new_symbol.name);  // Avoid duplicate insertion
            return NULL;
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

    while (ht->table[index].occupied) { // Only continue if the slot is occupied
        if (strcmp(ht->table[index].name, key) == 0) {
            return &ht->table[index]; // Return the address of the found symbol
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
            } else if (ht->table[index].type == SYM_VAR) {
                free(ht->table[index].data.var.value);
            }

            // Mark slot as unoccupied
            ht->table[index].occupied = false;
            ht->count--;
            return;
        }
        if (probe_count++ >= HASHTABLE_SIZE) break;
        index = (index + step) % HASHTABLE_SIZE;
    }
}