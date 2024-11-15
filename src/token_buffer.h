//FILE: token_buffer.h
//PROJEKT: IFJ24 - Compiler for the IFJ24 lang @ FIT BUT 2BIT
//TEAM: Martin Zůbek (253206)
//AUTHORS:
// Otakar Kočí (xkocio00, 247555)
//
//YEAR: 2024
//NOTES: Header file for token buffer. Works as Double Linked List for tokens.

#ifndef H_TOKEN_BUFFER
#define H_TOKEN_BUFFER

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "scanner.h"

// Node of the token buffer
typedef struct token_buffer_node {
    T_TOKEN *token;
    struct token_buffer_node *next;
    struct token_buffer_node *prev;
} T_TOKEN_BUFFER_NODE;

// Token buffer structure
typedef struct token_buffer {
    T_TOKEN_BUFFER_NODE *head;
    T_TOKEN_BUFFER_NODE *tail;
    T_TOKEN_BUFFER_NODE *current;
    T_TOKEN *dummy_eof_token;
} T_TOKEN_BUFFER;

// Function prototypes
T_TOKEN_BUFFER *init_token_buffer();
void free_token_buffer(T_TOKEN_BUFFER **buffer);
bool add_token_as_last(T_TOKEN_BUFFER *buffer, T_TOKEN *token);
void move_back(T_TOKEN_BUFFER *buffer);
void get_current_and_move_forward(T_TOKEN_BUFFER *buffer, T_TOKEN **token);
void set_current_to_first(T_TOKEN_BUFFER *buffer);

// for debug purposes
void print_token_buffer(T_TOKEN_BUFFER *buffer);



#endif //H_TOKEN_BUFFER