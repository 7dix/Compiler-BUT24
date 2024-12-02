// FILE: main_test_scanner.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Marek Tenora> (xtenor02)
//
// YEAR: 2024
// NOTES: Test Scanner with JSON output, example usage: ./main_test_scanner < input_file.ifj24 > output.json

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/scanner.h"
// Function to escape JSON strings
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
            snprintf(value_str, sizeof(value_str), "%d", token->value.intVal);
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

int main(int argc, char *argv[])
{
    int err_code = 0;
    T_TOKEN currentToken;
    int is_first_token = 1;

    if (argc == 2) {
        // Load file into stdin
        if (freopen(argv[1], "r", stdin) == NULL) {
            fprintf(stderr, "Error: Could not open input file %s\n", argv[1]);
            return 1;
        }
    }

    printf("[\n"); // Start of JSON array

    while (err_code == 0){
        err_code = get_token(&currentToken);
        if (err_code != 0){
            fprintf(stderr, "Error: Lexical error at line %d\n", currentToken.line);
            break;
        }
        if (currentToken.type == EOF_TOKEN){
            break;
        }

        // Print the token details
        print_token_json(&currentToken, is_first_token);
        is_first_token = 0;
    }
    printf("\n]\n"); // End of JSON array

    if (err_code != 0)
        fprintf(stderr, "..Compiling failed with exit code %d\n", err_code);
    return err_code;
}