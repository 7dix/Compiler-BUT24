// FILE: main_test_token_buffer.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Otakar Kočí> (xkocio00)
//  <Marek Tenora> (xtenor02) functions json_escape_string and print_token_json
//
// YEAR: 2024
// NOTES: Test Token Buffer with JSON output
//        Example usage: ./main_test_token_buffer < input_file.ifj24 > output.json

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../src/scanner.h"
#include "../../src/token_buffer.h"

// Function to escape JSON strings
// Copied from main_test_scanner.c
//@author Marek Tenora (xtenor02)
char* json_escape_string(const char* str) {
    if (str == NULL) {
        return strdup("null");
    }

    size_t len = strlen(str);
    // Allocate enough space (worst case all characters need escaping)
    char* escaped = malloc(len * 6 + 3); // Extra space for quotes and null terminator
    if (escaped == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in json_escape_string\n");
        exit(EXIT_FAILURE);
    }

    char* p = escaped;
    *p++ = '"';
    for (size_t i = 0; i < len; i++) {
        unsigned char c = str[i];
        switch (c) {
            case '\"': strcpy(p, "\\\""); p += 2; break;
            case '\\': strcpy(p, "\\\\"); p += 2; break;
            case '\b': strcpy(p, "\\b");  p += 2; break;
            case '\f': strcpy(p, "\\f");  p += 2; break;
            case '\n': strcpy(p, "\\n");  p += 2; break;
            case '\r': strcpy(p, "\\r");  p += 2; break;
            case '\t': strcpy(p, "\\t");  p += 2; break;
            default:
                if (c < 0x20 || c > 0x7F) {
                    sprintf(p, "\\u%04x", c);
                    p += 6;
                } else {
                    *p++ = c;
                }
                break;
        }
    }
    *p++ = '"';
    *p = '\0';

    return escaped;
}

// Function to print the token details in JSON format
// Copied from main_test_scanner.c
//@author Marek Tenora (xtenor02)
void print_token_json(T_TOKEN *token, int is_first) {
    if (token == NULL) {
        fprintf(stderr, "Error: token is NULL\n");
        return;
    }

    char *type_str = json_escape_string(tt_to_str(token->type));
    char *lexeme_str = json_escape_string(token->lexeme);
    char value_str[256] = "null";

    switch (token->type) {
        case INT:
            snprintf(value_str, sizeof(value_str), "%d", token->value.int_val);
            break;
        case FLOAT:
            snprintf(value_str, sizeof(value_str), "%f", token->value.float_val);
            break;
        case STRING:
            {
                char *escaped_str = json_escape_string(token->value.str_val);
                snprintf(value_str, sizeof(value_str), "%s", escaped_str);
                free(escaped_str);
            }
            break;
        default:
            strcpy(value_str, "null");
            break;
    }

    // Print comma if not the first element
    if (!is_first) {
        printf(",\n");
    }

    // Print the token as a JSON object
    printf("  {\n");
    printf("    \"type\": %s,\n", type_str);
    printf("    \"lexeme\": %s,\n", lexeme_str);
    printf("    \"line\": %d,\n", token->line);
    printf("    \"length\": %d,\n", token->length);
    printf("    \"value\": %s\n", value_str);
    printf("  }");

    free(type_str);
    free(lexeme_str);
}

int main()
{
    T_TOKEN_BUFFER *buffer = init_token_buffer();
    if (buffer == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in init_token_buffer\n");
        return 1;
    }



    int err_code = 0;
    T_TOKEN *currentToken;
    // Fill the buffer with tokens
    while (err_code == 0) {
        currentToken = (T_TOKEN *) malloc(sizeof(T_TOKEN));
        if (currentToken == NULL) {
            fprintf(stderr, "Error: Memory allocation failed in main\n");
            break;
        }
        err_code = get_token(currentToken);
        if (err_code != 0){
            fprintf(stderr, "Error: Lexical error at line %d\n", currentToken->line);
            break;
        }

        // Add the token to the buffer
        if (!add_token_as_last(buffer, currentToken)) {
            fprintf(stderr, "Error: Memory allocation failed in add_token_as_last\n");
            break;
        }

        if (currentToken->type == EOF_TOKEN){
            break;
        }
    }
    if (err_code != 0) {
        printf("\n]\n"); // End of JSON array
        fprintf(stderr, "..Compiling failed with exit code %d\n", err_code);
    }

    set_current_to_first(buffer);
    int is_first_token = 1;
    // Print the JSON array to check that the buffer is filled correctly
    printf("[\n"); // Start of JSON array

    // Buffer movement test
    move_back(buffer);
    move_back(buffer);
    
    for (int i = 0; i < 100; i++) {
        next_token(buffer, &currentToken);
    }

    for (int i = 0; i < 100; i++) {
        move_back(buffer);
    }

    for (int i = 0; i < 100; i++) {
        next_token(buffer, &currentToken);
    }

    set_current_to_first(buffer);



    // Print the buffer
    while (true) {
        next_token(buffer, &currentToken);

        if (currentToken->type == EOF_TOKEN)
            break;


        // Print the token details
        print_token_json(currentToken, is_first_token);
        is_first_token = 0;

    }

    printf("\n]\n"); // End of JSON array

    // Free the buffer
    free_token_buffer(&buffer);

    return err_code;
}