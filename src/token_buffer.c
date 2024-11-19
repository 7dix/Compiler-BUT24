//FILE: token_buffer.c
//PROJEKT: IFJ24 - Compiler for the IFJ24 lang @ FIT BUT 2BIT
//TEAM: Martin Zůbek (253206)
//AUTHORS:
// Otakar Kočí (xkocio00, 247555)
//
//YEAR: 2024
//NOTES: Implementation of the token buffer. Works as Double Linked List for tokens.
//       It is used as a place for tokens read during the first phase of the compilation.
//       When attempting to get a token, before the first one or after the last one
//       user gets either the first or the last token respectively.

#include "token_buffer.h"

/**
 * @brief Initializes the token buffer.
 * 
 * @return T_TOKEN_BUFFER* The initialized token buffer.
 */
T_TOKEN_BUFFER *init_token_buffer() {
    // Allocate memory and check for errors
    T_TOKEN_BUFFER *buffer = (T_TOKEN_BUFFER *) malloc(sizeof(T_TOKEN_BUFFER));
    if (buffer == NULL) {
        return NULL;
    }

    // Initialize the buffer
    buffer->head = NULL;
    buffer->tail = NULL;
    buffer->current = NULL;

    // Allocate memory for the dummy EOF token
    buffer->dummy_eof_token = (T_TOKEN *) malloc(sizeof(T_TOKEN));
    if (buffer->dummy_eof_token == NULL) {
        free(buffer);
        return NULL;
    }

    // Set the dummy EOF token
    buffer->dummy_eof_token->type = VOID_TOKEN;
    buffer->dummy_eof_token->lexeme = NULL;
    buffer->dummy_eof_token->line = 0;
    buffer->dummy_eof_token->length = 0;
    buffer->dummy_eof_token->value.intVal = 0;

    return buffer;
}

/**
 * @brief Frees the token buffer.
 * 
 * @param **buffer pointer to the token buffer to free.
 */
void free_token_buffer(T_TOKEN_BUFFER **buffer) {
    // Prepare pointers
    T_TOKEN_BUFFER_NODE *current = (*buffer)->head;
    T_TOKEN_BUFFER_NODE *next;

    // Free all nodes, one by one
    // also need to free the token its lexeme and string value
    while (current != NULL) {
        next = current->next;
        if (current->token->lexeme != NULL) {
            free(current->token->lexeme);
        }
        if (    (current->token->type == TYPE_STRING ||
                current->token->type == TYPE_STRING_NULL) &&
                current->token->value.stringVal != NULL
            ) {
            free(current->token->value.stringVal);
        }
        free(current->token);
        free(current);
        current = next;
    }
    // free the dummy EOF token
    free((*buffer)->dummy_eof_token);
    (*buffer)->dummy_eof_token = NULL;

    // Free the buffer itself
    free(*buffer);
    *buffer = NULL;
}

/**
 * @brief Adds a token to the end of the buffer.
 * 
 * @param  buffer The token buffer.
 * @param *token The token to add.
 * @return bool True if successful, false otherwise.
 */
bool add_token_as_last(T_TOKEN_BUFFER *buffer, T_TOKEN *token) {
    // Allocate memory for the new node
    T_TOKEN_BUFFER_NODE *new_node = (T_TOKEN_BUFFER_NODE *) malloc(sizeof(T_TOKEN_BUFFER_NODE));
    if (new_node == NULL) {
        return false;
    }

    // Link token
    new_node->token = token;

    // Set the pointers
    new_node->next = NULL;
    new_node->prev = buffer->tail;

    // Update the buffer
    if (buffer->head == NULL) {
        buffer->head = new_node;
    } else {
        buffer->tail->next = new_node;
    }
    buffer->tail = new_node;

    return true;
}

/**
 * @brief Moves the current pointer back.
 * 
 * @param *buffer The token buffer.
 */
void move_back(T_TOKEN_BUFFER *buffer) {
    // Move the current pointer back, if possible
    if (buffer->current != NULL && buffer->current->prev != NULL) {
        buffer->current = buffer->current->prev;
    }
}

/**
 * @brief Gets the current token and moves the current pointer forward.
 * 
 * @param *buffer The token buffer.
 * @param *token The token to store the current token.
 */
void next_token(T_TOKEN_BUFFER *buffer, T_TOKEN **token) {
    // Get the current token and move the current pointer forward
    if (buffer->current != NULL) { // Check if the current pointer is not NULL
        *token = buffer->current->token;

        // Move the current pointer forward, if possible
        if (buffer->current->next != NULL) {
            buffer->current = buffer->current->next;
        }
    } else { // If the current pointer is NULL, always return EOF token
        *token = buffer->dummy_eof_token;
    }
}

/**
 * @brief Sets the current pointer to the first token.
 * 
 * @param *buffer The token buffer.
 */
void set_current_to_first(T_TOKEN_BUFFER *buffer) {
    buffer->current = buffer->head;
}

/**
 * @brief Gets the last token in the buffer.
 * 
 * @param *buffer The token buffer.
 * @param **token The token to store the last token.
 */
void get_last_token(T_TOKEN_BUFFER *buffer, T_TOKEN **token) {
    if (buffer->tail == NULL) {
        *token = buffer->dummy_eof_token;
        return;
    }

    *token = buffer->tail->token;
}