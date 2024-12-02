#include <stdio.h>
#include <stdlib.h>
#include "../../src/first_phase.h"
#include "../../src/token_buffer.h"
#include "../../src/symtable.h"

T_SYM_TABLE *ST;

void print_type(VAR_TYPE type) {
    switch (type) {
        case VAR_INT:
            printf("i32");
            break;
        case VAR_FLOAT:
            printf("f64");
            break;
        case VAR_STRING:
            printf("[]u8");
            break;
        case VAR_INT_NULL:
            printf("?i32");
            break;
        case VAR_FLOAT_NULL:
            printf("?f64");
            break;
        case VAR_STRING_NULL:
            printf("?[]u8");
            break;
        case VAR_VOID:
            printf("void");
            break;
        default:
            printf("unknown");
            break;
    }
}

void print_function_signature(const char* name, const T_SYMBOL_DATA* data) {
    printf("pub fn %s(", name);
    
    // Print parameters
    for (int i = 0; i < data->func.argc; i++) {
        if (i > 0) {
            printf(", ");
        }
        printf("%s: ", data->func.argv[i].name);
        print_type(data->func.argv[i].type);
    }
    
    printf(") ");
    print_type(data->func.return_type);
    printf("\n");
}

void print_all_functions(T_SYM_TABLE *table) {
    if (!table || !table->top || !table->top->ht) {
        return;
    }

    // Iterate through symtable and print function signatures
    for (int i = 0; i < HASHTABLE_SIZE; i++) {
        // Get symbol directly from hashtable's table array
        if (table->top->ht->table[i].occupied && 
            table->top->ht->table[i].type == SYM_FUNC) {
            print_function_signature(
                table->top->ht->table[i].name,
                &table->top->ht->table[i].data
            );
        } else if (table->top->ht->table[i].occupied) {
            printf("T_SYMBOL %s is not a function\n", table->top->ht->table[i].name);
        }
    }
}

int main() {
    // Initialize token buffer
    T_TOKEN_BUFFER *token_buffer = init_token_buffer();
    if (token_buffer == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in init_token_buffer\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Initialize symtable
    T_SYM_TABLE *table = symtable_init();
    if (table == NULL) {
        free_token_buffer(&token_buffer);
        fprintf(stderr, "Error: Memory allocation failed in symtable_init\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Add global scope
    if (!symtable_add_scope(table)) {
        free_token_buffer(&token_buffer);
        symtable_free(table);
        fprintf(stderr, "Error: Memory allocation failed in symtable_add_scope\n");
        return RET_VAL_INTERNAL_ERR;
    }

    // Set global ST for first phase
    ST = table;

    // Run first phase
    int result = first_phase(token_buffer);
    if (result != RET_VAL_OK) {
        fprintf(stderr, "First phase failed with error code: %d\n", result);
        free_token_buffer(&token_buffer);
        symtable_free(table);
        return result;
    }

    // Print detected functions
    print_all_functions(table);

    // Cleanup
    free_token_buffer(&token_buffer);
    symtable_free(table);

    return RET_VAL_OK;
}