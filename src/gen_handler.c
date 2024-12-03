// FILE: gen_handler.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES: Code generation handler for the IFJ24 language compiler.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "generate.h"
#include "gen_handler.h"
#include "precedence_tree.h"
#include "symtable.h"
#include "semantic.h"

/***********************************************************************
 *                  FUNCTION HANDLERS & GENERATORS
 ***********************************************************************
 * Functions that handle the generation of the IFJ24 language instructions
 * for the provided functions, statements, expressions and other necessary
 * constructs.
 */

// Global counter for built-in functions requiring unique labels
int ord_counter = 0;
int strcmp_counter = 0;
int substr_counter = 0;

// While loop uniq counter
int while_counter = 0;
void while_append_uniq(char *input, char *output) {
    int len = snprintf(NULL, 0, "%s$%d", input, while_counter);
    output = (char *) malloc((len + 1) * sizeof(char));
    if (output != NULL) {
        sprintf(output, "%s$%d", input, while_counter);
    }
}

/**
 * @brief Creates the program header.
 * 
 * This function creates the program header for the IFJ24 language. The header includes
 * the necessary ".IFJcode24" directive and the definition of the necessary global variables.
 */
void create_program_header() {
    generate_header();
    generate_defvar("GF", "tmp1");
    generate_defvar("GF", "tmp2");
    generate_defvar("GF", "char");
    generate_defvar("GF", "char2");
    generate_defvar("GF", "valid");
    generate_defvar("GF", "trash");
    generate_defvar("GF", "index");
    generate_defvar("GF", "beg");
    generate_call("main");
    generate_exit(0);
}

/**
 * @brief Convert the provided indentifier to a unique identifier.
 * 
 * This function appends a unique identifier to the provided indentifier, creating
 * a unique identifier for the variable to be used in the local frame.
 * 
 * @param name The name of the variable.
 * 
 * @param uniq_name The unique name of the variable.
 */
void generate_unique_identifier(char *name, char **uniq_name) {
    int id = get_var_id(ST, name);
    size_t len = snprintf(NULL, 0, "%s$%d", name, id);
    *uniq_name = (char *) malloc((len + 1) * sizeof(char));
    if (*uniq_name != NULL) {
        sprintf(*uniq_name, "%s$%d", name, id);
    }
}

/**
 * @brief Creates a function header.
 * 
 * This function creates a function header for the provided function name and arguments.
 * 
 * @param name The name of the function.
 */
void create_fn_header(char *name) {
    if (name == NULL)
        return;
    T_SYMBOL *symbol = symtable_find_symbol(ST, name);
    generate_label(name);
    generate_create_frame();
    generate_push_frame();
    for (int i = symbol->data.func.argc - 1; i >= 0; i--) { // Reverse order iteration
        char *uniq = NULL;
        generate_unique_identifier(symbol->data.func.argv[i].name, &uniq);
        generate_defvar("LF", uniq);
        generate_pops("LF", uniq);
        free(uniq);
    }
}

/**
 * @brief Calls a function with the provided arguments.
 * 
 * This function pushes all arguments onto the interpreter stack and
 * calls the function with the provided name.
 * 
 * @param fn The function call to make.
 * 
 * @note The function also handles the conversion of the arguments to the correct type.
 */
void call_function(T_FN_CALL *fn) {
    for (int i = 0; i < fn->argc; i++) {
        if (fn->argv[i]->type == IDENTIFIER) {
            char *uniq = NULL;
            generate_unique_identifier(fn->argv[i]->lexeme, &uniq);
            generate_pushs("LF", uniq);
            free(uniq);
        }
        else if (fn->argv[i]->type == INT) {
            generate_pushs_int(fn->argv[i]->value.int_val);
        }
        else if (fn->argv[i]->type == FLOAT) {
            generate_pushs_float(fn->argv[i]->value.float_val);
        }
        else if (fn->argv[i]->type == STRING) {
            generate_pushs_string(fn->argv[i]->value.str_val);
        }
    }

    generate_call(fn->name);
}

/**
 * @brief Creates a return statement.
 * 
 * This function pops the current frame and creates a return statement.
 */
void create_return() {
    generate_pop_frame();
    generate_return();
}

/**
 * @brief Handles the discard of a variable.
 * 
 * This function handles the discard of a variable by moving the variable to the trash variable.
 * 
 * @note The trash variable is a special variable used to discard variables.
 */
void handle_discard() {
    generate_pops("GF", "trash");
}

/**
 * @brief Handles the definition of a unique variable.
 * 
 * This function handles the definition of a unique variable by generating a unique identifier
 * and defining the variable in the local frame.
 * 
 * @param var The variable to define.
 */
void handle_uniq_defvar(T_TOKEN *var) {
    char *uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_defvar("LF", uniq);
    free(uniq);
}

/**
 * @brief Traverses the expression tree in postorder and solves the expression.
 * 
 * This function traverses the expression tree in postorder, pushing the operands and operators
 * to the interpreter stack and solving the expression within the stack.
 * 
 * @param tree The expression tree to traverse.
 */
void solve_exp_by_postorder(T_TREE_NODE *tree) {

    if (tree == NULL) return; // Empty tree

    solve_exp_by_postorder(tree->left);
    solve_exp_by_postorder(tree->right);

    if (tree->token->type == IDENTIFIER) {
        char *uniq = NULL;
        generate_unique_identifier(tree->token->lexeme, &uniq);
        generate_pushs("LF", uniq);
        free(uniq);

        if (tree->convert_to_float) {
            generate_int2floats();
        }
        else if (tree->convert_to_int) {
            generate_float2ints();
        }
    }
    else if (tree->token->type == INT) {
        generate_pushs_int(tree->token->value.int_val);
        if (tree->convert_to_float) {
            generate_int2floats();
        }

    }
    else if (tree->token->type == FLOAT) {
        generate_pushs_float(tree->token->value.float_val);
        if (tree->convert_to_int) {
            generate_float2ints();
        }

    }
    else if (tree->token->type == PLUS) { // +
        generate_adds();
        if (tree->convert_to_float) {
            generate_int2floats();
        }
        else if (tree->convert_to_int) {
            generate_float2ints();
        }
    }
    else if (tree->token->type == MINUS) { // -
        generate_subs();
        if (tree->convert_to_float) {
            generate_int2floats();
        }
        else if (tree->convert_to_int) {
            generate_float2ints();
        }
    }
    else if (tree->token->type == MULTIPLY) { // *
        generate_muls();
        if (tree->convert_to_float) {
            generate_int2floats();
        }
        else if (tree->convert_to_int) {
            generate_float2ints();
        }
    }
    else if (tree->token->type == DIVIDE) { // /
        if (tree->result_type == TYPE_INT_RESULT) {
            generate_idivs();
        }
        else {
            generate_divs();
        }
        // Re-type
        if (tree->convert_to_float) {
            generate_int2floats();
        }
        else if (tree->convert_to_int) {
            generate_float2ints();
        }
    }
    else if (tree->token->type == NULL_TOKEN) {
        generate_pushs("nil", "nil");
    }
    else if (tree->token->type == LESS_THAN) { // <
        generate_lts();
    }
    else if (tree->token->type == GREATER_THAN) { // >
        generate_gts();
    }
    else if (tree->token->type == EQUAL) { // =
        generate_eqs();
    }
    else if (tree->token->type == NOT_EQUAL) { // !=
        generate_eqs();
        generate_nots();
    }
    else if (tree->token->type == LESS_THAN_EQUAL) { // <=
        generate_pops("GF", "tmp1");
        generate_pops("GF", "tmp2");
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_lts();
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_eqs();
        generate_ors();
    }
    else if (tree->token->type == GREATER_THAN_EQUAL) { // >=
        generate_pops("GF", "tmp1");
        generate_pops("GF", "tmp2");
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_gts();
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_eqs();
        generate_ors();
    }
    else if (tree->token->type == LESS_THAN) { // <
        generate_lts();
    }
    else if (tree->token->type == GREATER_THAN) { // >
        generate_gts();
    }
    else if (tree->token->type == EQUAL) { // =
        generate_eqs();
    }
    else if (tree->token->type == NOT_EQUAL) { // !=
        generate_eqs();
        generate_nots();
    }
    else if (tree->token->type == LESS_THAN_EQUAL) { // <=
        generate_pops("GF", "tmp1");
        generate_pops("GF", "tmp2");
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_lts();
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_eqs();
        generate_ors();
    }
    else if (tree->token->type == GREATER_THAN_EQUAL) { // >=
        generate_pops("GF", "tmp1");
        generate_pops("GF", "tmp2");
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_gts();
        generate_pushs("GF", "tmp2");
        generate_pushs("GF", "tmp1");
        generate_eqs();
        generate_ors();
    }
}

/**
 * @brief Assigns the current top of the stack to a unique variable.
 * 
 * This function calculates the unique variable ID and assigns the value that is present at the top
 * of the interpreter stack.
 * 
 * @param var The variable to be converted into an unique variable & assigned the value to.
 */
void handle_assign(char *var) {
    char *uniq = NULL;
    generate_unique_identifier(var, &uniq);
    generate_pops("LF", uniq);
    free(uniq);
}


/***********************************************************************
 *                         BUILT-IN FUNCTIONS
 ***********************************************************************
 * Functions that realize the defined built-in functions of the IFJ24
 * language provided in the documentation.
 */

/**
 * @brief Reads an integer from the input and stores it in the interpreter stack.
 * 
 * This function reads an integer from the input and stores it in the interpreter stack.
 */
void call_bi_readint() {
    generate_read("GF", "tmp1", "int");
    generate_pushs("GF", "tmp1");
}

/**
 * @brief Reads a floating-point number from the input and stores it in the interpreter stack.
 * 
 * This function reads a floating-point number from the input and stores it in the interpreter stack.
 */
void call_bi_readfloat() {
    generate_read("GF", "tmp1", "float");
    generate_pushs("GF", "tmp1");
}

/**
 * @brief Reads a string from the input and stores it in the interpreter stack.
 * 
 * This function reads a string from the input and stores it in the interpreter stack.
 */
void call_bi_readstring() {
    generate_read("GF", "tmp1", "string");
    generate_pushs("GF", "tmp1");
}

/**
 * @brief Convert an interger to a float.
 * 
 * This function handles the conversion of an integer to a floating-point number
 * and stores the result in the interpreter stack.
 * 
 * @param var The var to be converted.
 */
void call_bi_int2float(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generate_unique_identifier(var->lexeme, &uniq);
        generate_pushs("LF", uniq);
        free(uniq);
    }
    else if (var->type == INT) {
        generate_pushs_int(var->value.int_val);
    }
    generate_int2floats();
}

/**
 * @brief Converts a float to an integer.
 *
 * This function handles the conversion of a floating-point number to an integer
 * and stores the result in the interpreter stack.
 *
 * @param var The var to be converted.
 */
void call_bi_float2int(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generate_unique_identifier(var->lexeme, &uniq);
        generate_pushs("LF", uniq);
        free(uniq);
    }
    else if (var->type == FLOAT) {
        generate_pushs_float(var->value.float_val);
    }
    generate_float2ints();
}

/**
 * @brief Creates a variable for the provided string and stores the value.
 * 
 * This function handles creating a variable for the provided string and stores the value
 * into the variable.
 * 
 * @param var The var to be converted.
 */
void call_bi_string(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generate_unique_identifier(var->lexeme, &uniq);
        generate_pushs("LF", uniq);
        free(uniq);
    }
    else if (var->type == STRING) {
        generate_pushs_string(var->value.str_val);
    }
}

/**
 * @brief Calculates the length of a string.
 * 
 * This function calculates the length of a string and stores the result in the interpreter stack.
 * 
 * @param var The variable to calculate the length of.
 */
void call_bi_length(T_TOKEN *var) {
    char *uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_strlen("GF", "tmp1", "LF", uniq);
    generate_pushs("GF", "tmp1");
    free(uniq);
}

/**
 * @brief Concatenates two strings.
 * 
 * This function concatenates two strings and stores the result in the interpreter stack.
 * 
 * @param var The first variable (string).
 * @param _var The second variable.
 */
void call_bi_concat(T_TOKEN *var, T_TOKEN *_var) {
    char *uniq = NULL, *_uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_unique_identifier(_var->lexeme, &_uniq);
    generate_concat("GF", "tmp1", "LF", uniq, "LF", _uniq);
    generate_pushs("GF", "tmp1");
    free(uniq); free(_uniq);
}

/**
 * @brief Captures the defined character range of a string.
 * 
 * This function captures the defined character range of a string and stores the result in the interpreter stack.
 * 
 * @param var The variable to capture the substring from.
 * @param beg The beginning index of the substring.
 * @param end The ending index of the substring.
 * 
 * @note If the indexes are out of range or don't adhere to limitations, the destination variable will be set to nil.
 */
void call_bi_substring(T_TOKEN *var, T_TOKEN *beg, T_TOKEN *end) {

    char *_beg = NULL, *_end = NULL;
    char *uniq_beg = NULL, *uniq_end = NULL;
    size_t len = 0;

    char substr_loop[20];
    char substr_end[20];
    char substr_err[20];
    char substr_ret[20];
    sprintf(substr_loop, "substr_loop%d", substr_counter);
    sprintf(substr_end, "substr_end%d", substr_counter);
    sprintf(substr_err, "substr_err%d", substr_counter);
    sprintf(substr_ret, "substr_ret%d", substr_counter);


    if (beg->type == INT) {
        len = snprintf(_beg, 0, "int@%d", beg->value.int_val);
        _beg = (char *) malloc((len+1)*sizeof(char));
        sprintf(_beg, "int@%d", beg->value.int_val);
    }
    else if (beg->type == IDENTIFIER) {
        generate_unique_identifier(beg->lexeme, &uniq_beg);
        len = snprintf(_beg, 0, "LF@%s", uniq_beg);
        _beg = (char *) malloc((len+1)*sizeof(char));
        sprintf(_beg, "LF@%s", uniq_beg);
    }

    if (end->type == INT) {
        len = snprintf(_end, 0, "int@%d", end->value.int_val);
        _end = (char *) malloc((len+1)*sizeof(char));
        sprintf(_end, "int@%d", end->value.int_val);
    }
    else if (end->type == IDENTIFIER) {
        generate_unique_identifier(end->lexeme, &uniq_end);
        len = snprintf(_end, 0, "LF@%s", uniq_end);
        _end = (char *) malloc((len+1)*sizeof(char));
        sprintf(_end, "LF@%s", uniq_end);
    }

    char *uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);

    // Move global var value for iterating
    printf("MOVE GF@beg %s\n", _beg);
    generate_strlen("GF", "tmp1", "LF", uniq);

    // beg < 0
    printf("LT GF@valid %s int@0\n", _beg);
    generate_jumpifeq(substr_err, "GF", "valid", "bool", "true");

    // end < 0
    printf("LT GF@valid %s int@0\n", _end);
    generate_jumpifeq(substr_err, "GF", "valid", "bool", "true");

    // beg > end
    printf("GT GF@valid %s %s\n", _beg, _end);
    generate_jumpifeq(substr_err, "GF", "valid", "bool", "true");

    // beg >= length(var)
    printf("GT GF@valid %s GF@tmp1\n", _beg);
    printf("EQ GF@tmp2 %s GF@tmp1\n", _beg);
    printf("OR GF@valid GF@valid GF@tmp2\n");
    generate_jumpifeq(substr_err, "GF", "valid", "bool", "true");

    // end > length(var)
    printf("GT GF@valid %s GF@tmp1\n", _end);
    generate_jumpifeq(substr_err, "GF", "valid", "bool", "true");

    // move "string@" to dest
    printf("MOVE GF@tmp2 string@\n");

    // Loop
    generate_label(substr_loop);

    // check if end is reached (beg == end)
    printf("EQ GF@valid GF@beg %s\n", _end);
    generate_jumpifeq(substr_end, "GF", "valid", "bool", "true");
    printf("GT GF@valid GF@beg %s\n", _end);
    generate_jumpifeq(substr_end, "GF", "valid", "bool", "true");

    // get char
    printf("GETCHAR GF@char LF@%s GF@beg\n", uniq);
    generate_concat("GF", "tmp2", "GF", "tmp2", "GF", "char");

    // increment beg
    printf("ADD GF@beg GF@beg int@1\n");

    generate_jump(substr_loop);

    // End of loop
    generate_label(substr_end);
    generate_pushs("GF", "tmp2");
    generate_jump(substr_ret);

    // Error handling
    generate_label(substr_err);
    generate_move("GF", "tmp2", "nil", "nil");
    generate_pushs("GF", "tmp2");
    generate_jump(substr_ret);

    // Return label
    generate_label(substr_ret);
    free(uniq_beg); free(uniq_end);
    free(uniq); free(_beg); free(_end);

    substr_counter++;
} 

/**
 * @brief Compares two string stored in variables and stores the result in a destination variable.
 * 
 * This function compares two source strings stored in variables, evaluates them and stores the result
 * in the destination variable.
 * 
 * @param var The first variable (string).
 * @param _var The second variable.
 * 
 * @note The destination variable will be set to 0 if the strings are equal, -1 if the first string is lesser and 1 if the first string is greater.
 */
void call_bi_strcmp(T_TOKEN *var, T_TOKEN *_var) {
    char *uniq = NULL, *_uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_unique_identifier(_var->lexeme, &_uniq);

    char strcmp_end_length[20];
    char strcmp_ret_lesser[20];
    char strcmp_ret[20];
    sprintf(strcmp_end_length, "strcmp_end_length%d", strcmp_counter);
    sprintf(strcmp_ret_lesser, "strcmp_ret_lesser%d", strcmp_counter);
    sprintf(strcmp_ret, "strcmp_ret%d", strcmp_counter);

    // Push strings to stack
    generate_pushs("LF", uniq);
    generate_pushs("LF", _uniq);

    // IF S1 != S2
    generate_jumpifneqs(strcmp_end_length);

    // S1 = S2
    generate_pushs("int", "0");
    generate_jump(strcmp_ret);

    // S1 != S2
    generate_label(strcmp_end_length);
    generate_pushs("LF", uniq);
    generate_pushs("LF", _uniq);
    generate_lts(); // S1 < S2
    generate_pushs("bool", "true");
    generate_jumpifeqs(strcmp_ret_lesser); // JMP IF: S1 < S2
    generate_pushs("int", "1");
    generate_jump(strcmp_ret);

    // S1 < S2 SCENARIO
    generate_label(strcmp_ret_lesser);
    generate_pushs("int", "-1");
    generate_jump(strcmp_ret);

    // Return label, free memory
    generate_label(strcmp_ret);
    free(uniq); free(_uniq);

    strcmp_counter++;
}

/**
 * @brief Converts the char at the provided index to an int.
 * 
 * This function converts the character at the provided index to an integer and stores the result in the interpreter stack.
 * 
 * @param var The source string variable.
 * @param index The index of the character to convert.
 */
void call_bi_ord (T_TOKEN *var, T_TOKEN *index) {
    char *uniq = NULL, *_index = NULL;
    size_t len = 0;

    char ord_err[20];
    char ord_ret[20];

    sprintf(ord_err, "ord_err%d", ord_counter);
    sprintf(ord_ret, "ord_ret%d", ord_counter);

    if (index->type == INT) {
        len = snprintf(_index, 0, "int@%d", index->value.int_val);
        _index = (char *) malloc((len+1)*sizeof(char));
        sprintf(_index, "int@%d", index->value.int_val);
    }
    else if (index->type == IDENTIFIER) {
        generate_unique_identifier(index->lexeme, &uniq);
        len = snprintf(_index, 0, "LF@%s", uniq);
        _index = (char *) malloc((len+1)*sizeof(char));
        sprintf(_index, "LF@%s", uniq);
        free(uniq);
    }

    generate_unique_identifier(var->lexeme, &uniq);

    generate_strlen("GF", "tmp1", "LF", uniq);

    // strlen(var) = 0
    generate_jumpifeq(ord_err, "GF", "tmp1", "int", "0");

    // index < strlen(var)
    printf("LT GF@valid %s %s\n", _index, "GF@tmp1");
    generate_jumpifeq(ord_err, "GF", "valid", "bool", "false");

    // index < 0
    printf("LT GF@valid %s int@0\n", _index);
    generate_jumpifeq(ord_err, "GF", "valid", "bool", "true");

    // Push string and post to stack, evaluate
    generate_pushs("LF", uniq);
    printf("PUSHS %s\n", _index);
    generate_stri2ints();
    generate_jump(ord_ret);

    // Error label
    generate_label(ord_err);
    generate_pushs("int", "0");

    // Return label
    generate_label(ord_ret);
    free(uniq); free(_index);

    ord_counter++;
}

/**
 * @brief Converts an integer to a char.
 * 
 * This function converts an integer to a character and stores the result in the interpreter stack.
 * 
 * @param var The integer to convert.
 */
void call_bi_chr(T_TOKEN *var) {
    char *uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_pushs("LF", uniq);
    generate_int2chars();
    free(uniq);
}

/**
 * @brief Prints the provided variable to the output.
 * 
 * This function prints the provided variable to standard output.
 * 
 * @param var The variable to print
 */
void call_bi_write(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generate_unique_identifier(var->lexeme, &uniq);
        generate_write("LF", uniq);
        free(uniq);
    }
    else if (var->type == INT) {
        printf("WRITE int@%d\n", var->value.int_val);
    }
    else if (var->type == FLOAT) {
        printf("WRITE float@%a\n", var->value.float_val);
    }
    else if (var->type == STRING) {
        char *out = NULL;
        handle_correct_string_format(var->value.str_val, &out);
        generate_write("string", out);
        free(out);
    }
    else if (var->type == NULL_TOKEN) {
        printf("WRITE nil@nil\n");
    }
}

/**
 * @brief Calls the built-in function based on the provided function call.
 * 
 * This function calls the built-in function based on the provided function call.
 * 
 * @param fn The function call to handle.
 */
void call_bi_fn(T_FN_CALL *fn) {
    if (strcmp(fn->name, "ifj.readstr") == 0) {
        call_bi_readstring();
    }
    else if (strcmp(fn->name, "ifj.readi32") == 0) {
        call_bi_readint();
    }
    else if (strcmp(fn->name, "ifj.readf64") == 0) {
        call_bi_readfloat();
    }
    else if (strcmp(fn->name, "ifj.write") == 0) {
        call_bi_write(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.i2f") == 0) {
        call_bi_int2float(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.f2i") == 0) {
        call_bi_float2int(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.string") == 0) {
        call_bi_string(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.length") == 0) {
        call_bi_length(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.concat") == 0) {
        call_bi_concat(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.substring") == 0) {
        call_bi_substring(fn->argv[0], fn->argv[1], fn->argv[2]);
    }
    else if (strcmp(fn->name, "ifj.strcmp") == 0) {
        call_bi_strcmp(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.ord") == 0) {
        call_bi_ord(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.chr") == 0) {
        call_bi_chr(fn->argv[0]);
    }
}

/***********************************************************************
 *                         CONDITIONS & LOOPS
 ***********************************************************************
 * Functions to handle conditions and loops
 * Before calling each, both the label and expression
 * solution must be generated.
 */

/**
 * @brief Handles the start of an if statement with a boolean expression.
 * 
 * This function handles the start of an if statement with a boolean expression, jumps
 * accordingly to the label_else if the expression is False.
 * 
 * @param label_else The label to jump to.
 * @param upper The upper flow control block ID
 * @param current The current flow control block ID
 * 
 * @note The expression must be solved before calling this function.
 */
void handle_if_start_bool(char *label_else, int upper, int current) {
    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@defined$%d bool@true\n", label_counter, upper);
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
        printf("LABEL skipDefvar$%d\n", label_counter);
        label_counter++;
    }
    else {
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
    }

    generate_pops("GF", "tmp1");
    generate_jumpifneq(label_else, "GF", "tmp1", "bool", "true");
}

/**
 * @brief Handles the start of an if statement with a numerical expression.
 * 
 * This function handles the start of an if statement with a numerical expression, jumps
 * accordingly to the label_else if the expression is NULL (end of loop).
 * 
 * @param label_else The label to jump to.
 * @param var The variable to store the expression result.
 * @param upper The upper flow control block ID
 * @param current The current flow control block ID
 * 
 * @note The expression must be solved before calling this function.
 */
void handle_if_start_nil(char *label_else, T_TOKEN *var, int upper, int current) {
    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@defined$%d bool@true\n", label_counter, upper);
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
        printf("LABEL skipDefvar$%d\n", label_counter);
        label_counter++;
    }
    else {
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
    }

    generate_pops("GF", "tmp1");
    generate_type("GF", "tmp2", "GF", "tmp1");
    generate_jumpifeq(label_else, "GF", "tmp2", "string", "nil");

    char *uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_defvar("LF", uniq);
    generate_move("LF", uniq, "GF", "tmp1");
    free(uniq);
}

/**
 * @brief Creates the else part of an if statement.
 * 
 * This function creates the else part of an if statement and jumps to the end of the if statement.
 * 
 * @param label_end The end label to jump to.
 * @param label_else The else label to jump to.
 * @param upper The upper flow control block ID
 * @param current_if The current if block ID
 * @param current_else The current else block ID
 */
void create_if_else(char *label_end, char *label_else, int upper, int current_if, int current_else) {
    printf("MOVE LF@defined$%d bool@true\n", current_if);

    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@defined$%d bool@true\n", label_counter, upper);
        printf("DEFVAR LF@defined$%d\n", current_else);
        printf("MOVE LF@defined$%d bool@false\n", current_else);
        printf("LABEL skipDefvar$%d\n", label_counter);
        label_counter++;
    }
    else {
        printf("DEFVAR LF@defined$%d\n", current_else);
        printf("MOVE LF@defined$%d bool@false\n", current_else);
    }

    generate_jump(label_end);
    generate_label(label_else);

    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@defined$%d bool@true\n", label_counter, upper);
        printf("DEFVAR LF@defined$%d\n", current_else);
        printf("MOVE LF@defined$%d bool@false\n", current_else);
        printf("LABEL skipDefvar$%d\n", label_counter);
        label_counter++;
    }
    else {
        printf("DEFVAR LF@defined$%d\n", current_else);
        printf("MOVE LF@defined$%d bool@false\n", current_else);
    }
}

/**
 * @brief Creates the end of an if statement.
 * 
 * This function creates the end of an if statement.
 * 
 * @param label_end The end label to jump to.
 * @param current The current flow control block ID
 */
void create_if_end(char *label_end, int current) {
    printf("MOVE LF@defined$%d bool@true\n", current);
    generate_label(label_end);
}

/**
 * @brief Creates the start of a while statement with a boolean expression.
 * 
 * This function creates the start of a while statement with a boolean expression.
 * 
 * @param label_start The start label to jump to.
 * @param upper The upper flow control block ID
 * @param current The current flow control block ID
 */
void create_while_bool_header(char *label_start, int upper, int current) { // Upper > -1, not inside of while, >= 0 inside of while (ID OF TOP WHILE)
    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@defined$%d bool@true\n", label_counter, upper);
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
        printf("LABEL skipDefvar$%d\n", label_counter);
        label_counter++;
    }
    else {
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
    }
    generate_label(label_start);
}

/**
 * @brief Creates the start of a while statement with a numerical expression.
 * 
 * This function creates the start of a while statement with a numerical expression.
 * 
 * @param label_start The start label to jump to.
 * @param var The variable to store the expression result.
 * @param upper The upper flow control block ID
 * @param current The current flow control block ID
 */
void create_while_nil_header(char *label_start, T_TOKEN *var, int upper, int current) {
    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@defined$%d bool@true\n", label_counter, upper);
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
        printf("LABEL skipDefvar$%d\n", label_counter);
        label_counter++;
    }
    else {
        printf("DEFVAR LF@defined$%d\n", current);
        printf("MOVE LF@defined$%d bool@false\n", current);
    }

    char *uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_defvar("LF", uniq);
    free(uniq);

    generate_label(label_start);
}

/**
 * @brief Handles the while statement with a boolean expression.
 * 
 * This function handles the while statement with a boolean expression.
 * 
 * @param label_end The end label to jump to.
 */
void handle_while_bool(char *label_end) {
    generate_pops("GF", "tmp1");
    generate_jumpifneq(label_end, "GF", "tmp1", "bool", "true");
}

/**
 * @brief Handles the while statement with a numerical expression.
 * 
 * This function handles the while statement with a numerical expression.
 * 
 * @param label_end The end label to jump to.
 * @param var The variable to store the expression result.
 */
void handle_while_nil(char *label_end, T_TOKEN *var) {
    generate_pops("GF", "tmp1");
    generate_type("GF", "tmp2", "GF", "tmp1");
    generate_jumpifeq(label_end, "GF", "tmp2", "string", "nil");

    char *uniq = NULL;
    generate_unique_identifier(var->lexeme, &uniq);
    generate_move("LF", uniq, "GF", "tmp1");
    free(uniq);
}

/**
 * @brief Creates the end of a while statement.
 * 
 * This function creates the end of a while statement.
 * 
 * @param label_start The start label to jump to.
 * @param label_end The end label to jump to.
 * @param while_def_counter The current while block definition counter ID
 */
void create_while_end(char *label_start, char *label_end, int while_def_counter) {
    printf("MOVE LF@defined$%d bool@true\n", while_def_counter);
    generate_jump(label_start);
    generate_label(label_end);
}