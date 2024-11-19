

#include <stdio.h>
#include <stdlib.h>
#include "symtable.h"

int main() {
    T_SYM_TABLE *table = symtable_init();
    if (table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in symtable_init\n");
        return 1;
    }

    if (!symtable_add_scope(table)) {
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_scope\n");
        return 1;
    }
    char *test = strdup("test");
    SymbolData data = {
        .var = { 
            .is_const = false,
            .type = VAR_INT,
            .value = NULL }
        };
    Symbol *symbol = symtable_add_symbol(table, test, SYM_VAR, data);
    if (symbol == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_symbol\n");
        return 1;
    }

    symbol = symtable_find_symbol(table, test);
    if (symbol == NULL) {
        fprintf(stderr, "Error: Symbol that should exist was not found\n");
        return 1;
    }
    symbol = symtable_find_symbol(table, "nonexistent");
    if (symbol != NULL) {
        fprintf(stderr, "Error: Symbol that should not exist was found\n");
        return 1;
    }

    if (!symtable_add_scope(table)) {
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_scope\n");
        return 1;
    }

    test = strdup("test2");
    data = (SymbolData) {
        .var = { 
            .is_const = false,
            .type = VAR_INT,
            .value = NULL }
        };
    symbol = symtable_add_symbol(table, test, SYM_VAR, data);
    if (symbol == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_symbol\n");
        return 1;
    }

    symbol = symtable_find_symbol(table, test);
    if (symbol == NULL) {
        fprintf(stderr, "Error: Symbol that should exist was not found\n");
        return 1;
    }

    symbol = symtable_find_symbol(table, "nonexistent");
    if (symbol != NULL) {
        fprintf(stderr, "Error: Symbol that should not exist was found\n");
        return 1;
    }

    symbol = symtable_find_symbol(table, "test2");
    if (symbol == NULL) {
        fprintf(stderr, "Error: Symbol that should exist was not found\n");
        return 1;
    }

    symtable_remove_scope(table);
    symtable_add_scope(table);

    // Fill up the hashtable
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        char key[10];
        sprintf(key, "key%d", i);
        data = (SymbolData) {
            .var = { 
                .is_const = false,
                .type = VAR_INT,
                .value = NULL }
            };
        symbol = symtable_add_symbol(table, key, SYM_VAR, data);
        if (symbol == NULL) {
            fprintf(stderr, "Error: Memory allocation failed in symtable_add_symbol, iteration: %d\n", i);
            return 1;
        }
    }



    symtable_remove_scope(table);
    symtable_remove_scope(table);

    symtable_free(table);
    return 0;
}