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
void whileAppendUniq(char *input, char *output) {
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
void createProgramHeader() {
    generateHeader();
    generateDefvar("GF", "tmp1");
    generateDefvar("GF", "tmp2");
    generateDefvar("GF", "char");
    generateDefvar("GF", "char2");
    generateDefvar("GF", "valid");
    generateDefvar("GF", "trash");
    generateDefvar("GF", "beg");
    generateDefvar("GF", "index");
    generateCall("main");
    generateExit(0);
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
void generateUniqueIdentifier(char *name, char **uniq_name) {
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
 * @param args The arguments of the function.
 * @param argCount The total number of arguments.
 */
void createFnHeader(char *name) {
    if (name == NULL)
        return;
    Symbol *symbol = symtable_find_symbol(ST, name);
    generateLabel(name);
    generateCreateFrame();
    generatePushFrame();
    for (int i = symbol->data.func.argc - 1; i >= 0; i--) { // Reverse order iteration
        char *uniq = NULL;
        generateUniqueIdentifier(symbol->data.func.argv[i].name, &uniq);
        generateDefvar("LF", uniq);
        generatePops("LF", uniq);
        free(uniq);
    }
}

/**
 * @brief Calls a function with the provided arguments.
 * 
 * This function pushes all arguments onto the interpreter stack and
 * calls the function with the provided name.
 * 
 * @param name The name of the function to call.
 * @param args The arguments to push onto the stack & pass to the function.
 * @param argCount The total number of arguments.
 * 
 * @note The function also handles the conversion of the arguments to the correct type.
 */
void callFunction(T_FN_CALL *fn) {
    for (int i = 0; i < fn->argc; i++) {
        if (fn->argv[i]->type == IDENTIFIER) {
            char *uniq = NULL;
            generateUniqueIdentifier(fn->argv[i]->lexeme, &uniq);
            generatePushs("LF", uniq);
            free(uniq);
        }
        else if (fn->argv[i]->type == INT) {
            generatePushsInt(fn->argv[i]->value.intVal);
        }
        else if (fn->argv[i]->type == FLOAT) {
            generatePushsFloat(fn->argv[i]->value.floatVal);
        }
        else if (fn->argv[i]->type == STRING) {
            generatePushsString(fn->argv[i]->value.stringVal);
        }
    }

    generateCall(fn->name);
}

/**
 * @brief Creates a return statement.
 * 
 * This function pops the current frame and creates a return statement.
 */
void createReturn() {
    generatePopFrame();
    generateReturn();
}

/**
 * @brief Handles the discard of a variable.
 * 
 * This function handles the discard of a variable by moving the variable to the trash variable.
 * 
 * @note The trash variable is a special variable used to discard variables.
 */
void handleDiscard() {
    generatePops("GF", "trash");
}

/**
 * @brief Handles the definition of a unique variable.
 * 
 * This function handles the definition of a unique variable by generating a unique identifier
 * and defining the variable in the local frame.
 * 
 * @param var The variable to define.
 */
void handleUniqDefvar(T_TOKEN *var) {
    char *uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generateDefvar("LF", uniq);
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
void createStackByPostorder(T_TREE_NODE *tree) {

    if (tree == NULL) return; // Empty tree

    createStackByPostorder(tree->left);
    createStackByPostorder(tree->right);

    if (tree->token->type == IDENTIFIER) {
        char *uniq = NULL;
        generateUniqueIdentifier(tree->token->lexeme, &uniq);
        generatePushs("LF", uniq);
        free(uniq);

    }
    else if (tree->token->type == INT) {
        generatePushsInt(tree->token->value.intVal);
        if (tree->convertToFloat) {
            generateInt2Floats();
        }

    }
    else if (tree->token->type == FLOAT) {
        generatePushsFloat(tree->token->value.floatVal);
        if (tree->convertToInt) {
            generateFloat2Ints();
        }

    }
    else if (tree->token->type == PLUS) { // +
        generateAdds();
        if (tree->convertToFloat) {
            generateInt2Floats();
        }
        else if (tree->convertToInt) {
            generateFloat2Ints();
        }
    }
    else if (tree->token->type == MINUS) { // -
        generateSubs();
        if (tree->convertToFloat) {
            generateInt2Floats();
        }
        else if (tree->convertToInt) {
            generateFloat2Ints();
        }
    }
    else if (tree->token->type == MULTIPLY) { // *
        generateMuls();
        if (tree->convertToFloat) {
            generateInt2Floats();
        }
        else if (tree->convertToInt) {
            generateFloat2Ints();
        }
    }
    else if (tree->token->type == DIVIDE) { // /
        generateDivs();
        if (tree->convertToFloat) {
            generateInt2Floats();
        }
        else if (tree->convertToInt) {
            generateFloat2Ints();
        }
    }
    else if (tree->token->type == NULL_TOKEN) {
        generatePushs("nil", "nil");
    }
    else if (tree->token->type == LESS_THAN) { // <
        generateLts();
    }
    else if (tree->token->type == GREATER_THAN) { // >
        generateGts();
    }
    else if (tree->token->type == EQUAL) { // =
        generateEqs();
    }
    else if (tree->token->type == NOT_EQUAL) { // !=
        generateEqs();
        generateNots();
    }
    else if (tree->token->type == LESS_THAN_EQUAL) { // <=
        generatePops("GF", "tmp1");
        generatePops("GF", "tmp2");
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateLts();
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateEqs();
        generateOrs();
    }
    else if (tree->token->type == GREATER_THAN_EQUAL) { // >=
        generatePops("GF", "tmp1");
        generatePops("GF", "tmp2");
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateGts();
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateEqs();
        generateOrs();
    }
    else if (tree->token->type == LESS_THAN) { // <
        generateLts();
    }
    else if (tree->token->type == GREATER_THAN) { // >
        generateGts();
    }
    else if (tree->token->type == EQUAL) { // =
        generateEqs();
    }
    else if (tree->token->type == NOT_EQUAL) { // !=
        generateEqs();
        generateNots();
    }
    else if (tree->token->type == LESS_THAN_EQUAL) { // <=
        generatePops("GF", "tmp1");
        generatePops("GF", "tmp2");
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateLts();
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateEqs();
        generateOrs();
    }
    else if (tree->token->type == GREATER_THAN_EQUAL) { // >=
        generatePops("GF", "tmp1");
        generatePops("GF", "tmp2");
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateGts();
        generatePushs("GF", "tmp2");
        generatePushs("GF", "tmp1");
        generateEqs();
        generateOrs();
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
void handleAssign(char *var) {
    char *uniq = NULL;
    generateUniqueIdentifier(var, &uniq);
    generatePops("LF", uniq);
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
void callBIReadInt() {
    generateRead("GF", "tmp1", "int");
    generatePushs("GF", "tmp1");
}

/**
 * @brief Reads a floating-point number from the input and stores it in the interpreter stack.
 * 
 * This function reads a floating-point number from the input and stores it in the interpreter stack.
 */
void callBIReadFloat() {
    generateRead("GF", "tmp1", "float");
    generatePushs("GF", "tmp1");
}

/**
 * @brief Reads a string from the input and stores it in the interpreter stack.
 * 
 * This function reads a string from the input and stores it in the interpreter stack.
 */
void callBIReadString() {
    generateRead("GF", "tmp1", "string");
    generatePushs("GF", "tmp1");
}

/**
 * @brief Convert an interger to a float.
 * 
 * This function handles the conversion of an integer to a floating-point number
 * and stores the result in the interpreter stack.
 * 
 * @param var The var to be converted.
 */
void callBIInt2Float(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generateUniqueIdentifier(var->lexeme, &uniq);
        generatePushs("LF", uniq);
        free(uniq);
    }
    else if (var->type == INT) {
        generatePushsInt(var->value.intVal);
    }
    generateInt2Floats();
}

/**
 * @brief Converts a float to an integer.
 *
 * This function handles the conversion of a floating-point number to an integer
 * and stores the result in the interpreter stack.
 *
 * @param var The var to be converted.
 */
void callBIFloat2Int(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generateUniqueIdentifier(var->lexeme, &uniq);
        generatePushs("LF", uniq);
        free(uniq);
    }
    else if (var->type == FLOAT) {
        generatePushsFloat(var->value.floatVal);
    }
    generateFloat2Ints();
}

/**
 * @brief Creates a variable for the provided string and stores the value.
 * 
 * This function handles creating a variable for the provided string and stores the value
 * into the variable.
 * 
 * @param var The var to be converted.
 */
void callBIString(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generateUniqueIdentifier(var->lexeme, &uniq);
        generatePushs("LF", uniq);
        free(uniq);
    }
    else if (var->type == STRING) {
        generatePushsString(var->value.stringVal);
    }
}

/**
 * @brief Calculates the length of a string.
 * 
 * This function calculates the length of a string and stores the result in the interpreter stack.
 * 
 * @param var The variable to calculate the length of.
 */
void callBILength(T_TOKEN *var) {
    char *uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generateStrlen("GF", "tmp1", "LF", uniq);
    generatePushs("GF", "tmp1");
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
void callBIConcat(T_TOKEN *var, T_TOKEN *_var) {
    char *uniq = NULL, *_uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generateUniqueIdentifier(_var->lexeme, &_uniq);
    generateConcat("GF", "tmp1", "LF", uniq, "LF", _uniq);
    generatePushs("GF", "tmp1");
    free(uniq); free(_uniq);
}

/**
 * @brief Gets the character at the provided index range and stores it in a variable.
 * 
 * This function gets the character at the provided index range and stores it in a defined variable
 * at within a given frame.
 * 
 * @param frame The frame of the source variable.
 * @param var The source variable.
 * @param beg The beginning index of the character to get.
 * @param end The ending index of the character to get.
 * @param _frame The frame of the destination variable.
 * @param _dest The destination variable.
 * 
 * @note If the indexes are out of range or don't adhere to limitations, the destination variable will be set to nil.
 */
void callBISubstring(T_TOKEN *var, T_TOKEN *beg, T_TOKEN *end) {

    char *_beg = NULL, *_end = NULL;
    char *uniq_beg = NULL, *uniq_end = NULL;
    size_t len = 0;

    char substr_loop[20];
    char substr_end[20];
    char substr_err[20];
    char substr_ret[20];
    sprintf(substr_loop, "strcmp_loop%d", substr_counter);
    sprintf(substr_end, "strcmp_end%d", substr_counter);
    sprintf(substr_err, "strcmp_err%d", substr_counter);
    sprintf(substr_ret, "strcmp_ret%d", substr_counter);


    if (beg->type == INT) {
        len = snprintf(_beg, 0, "int@%d", beg->value.intVal);
        _beg = (char *) malloc((len+1)*sizeof(char));
        sprintf(_beg, "int@%d", beg->value.intVal);
    }
    else if (beg->type == IDENTIFIER) {
        generateUniqueIdentifier(beg->lexeme, &uniq_beg);
        len = snprintf(_beg, 0, "LF@%s", uniq_beg);
        _beg = (char *) malloc((len+1)*sizeof(char));
        sprintf(_beg, "LF@%s", uniq_beg);
    }

    if (end->type == INT) {
        len = snprintf(_end, 0, "int@%d", end->value.intVal);
        _end = (char *) malloc((len+1)*sizeof(char));
        sprintf(_end, "int@%d", end->value.intVal);
    }
    else if (end->type == IDENTIFIER) {
        generateUniqueIdentifier(end->lexeme, &uniq_end);
        len = snprintf(_end, 0, "LF@%s", uniq_end);
        _end = (char *) malloc((len+1)*sizeof(char));
        sprintf(_end, "LF@%s", uniq_end);
    }

    char *uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);

    generateStrlen("GF", "tmp1", "LF", uniq);

    // beg < 0
    printf("LT GF@valid int@0 %s\n", _beg);
    generateJumpifeq(substr_err, "GF", "valid", "bool", "true");

    // end < 0
    printf("LT GF@valid int@0 %s\n", _end);
    generateJumpifeq(substr_err, "GF", "valid", "bool", "true");

    // beg > end
    printf("GT GF@valid %s %s\n", _beg, _end);
    generateJumpifeq(substr_err, "GF", "valid", "bool", "true");

    // beg >= length(var)
    printf("GT GF@valid %s GF@tmp1\n", _beg);
    printf("EQ GF@tmp2 %s GF@tmp1\n", _beg);
    printf("OR GF@valid GF@valid GF@tmp2\n");
    generateJumpifeq(substr_err, "GF", "valid", "bool", "true");

    // end > length(var)
    printf("GT GF@valid %s GF@tmp1\n", _end);
    generateJumpifeq(substr_err, "GF", "valid", "bool", "true");

    // move "string@" to dest
    printf("MOVE GF@tmp2 string@\n");

    // Loop
    generateLabel(substr_loop);
    printf("LT GF@valid %s %s\n", _beg, _end);
    generateJumpifeq(substr_end, "GF", "valid", "bool", "false");

    // get char
    printf("GETCHAR GF@char LF@%s %s\n", uniq, _beg);
    generateConcat("GF", "tmp2", "GF", "tmp2", "GF", "char");

    // increment beg
    printf("ADD GF@beg %s int@1\n", _beg);
    generateJump(substr_loop);

    // End of loop
    generateLabel(substr_end);
    generatePushs("GF", "tmp2");
    generateJump(substr_ret);

    // Error handling
    generateLabel(substr_err);
    generateMove("GF", "tmp2", "nil", "nil");
    generatePushs("GF", "tmp2");
    generateJump(substr_ret);

    // Return label
    generateLabel(substr_ret);
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
void callBIStrcmp(T_TOKEN *var, T_TOKEN *_var) {
    char *uniq = NULL, *_uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generateUniqueIdentifier(_var->lexeme, &_uniq);

    char strcmp_end_length[20];
    char strcmp_ret_lesser[20];
    char strcmp_ret[20];
    sprintf(strcmp_end_length, "strcmp_end_length%d", strcmp_counter);
    sprintf(strcmp_ret_lesser, "strcmp_ret_lesser%d", strcmp_counter);
    sprintf(strcmp_ret, "strcmp_ret%d", strcmp_counter);

    // Push strings to stack
    generatePushs("LF", uniq);
    generatePushs("LF", _uniq);

    // IF S1 != S2
    generateJumpifneqs(strcmp_end_length);

    // S1 = S2
    generatePushs("int", "0");
    generateJump(strcmp_ret);

    // S1 != S2
    generateLabel(strcmp_end_length);
    generatePushs("LF", uniq);
    generatePushs("LF", _uniq);
    generateLts(); // S1 < S2
    generatePushs("bool", "true");
    generateJumpifeqs(strcmp_ret_lesser); // JMP IF: S1 < S2
    generatePushs("int", "1");
    generateJump(strcmp_ret);

    // S1 < S2 SCENARIO
    generateLabel(strcmp_ret_lesser);
    generatePushs("int", "-1");
    generateJump(strcmp_ret);

    // Return label, free memory
    generateLabel(strcmp_ret);
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
void callBIOrd (T_TOKEN *var, T_TOKEN *index) {
    char *uniq = NULL, *_index = NULL;
    size_t len = 0;

    char ord_err[20];
    char ord_ret[20];

    sprintf(ord_err, "ord_err%d", ord_counter);
    sprintf(ord_ret, "ord_ret%d", ord_counter);

    if (index->type == INT) {
        len = snprintf(_index, 0, "int@%d", index->value.intVal);
        _index = (char *) malloc((len+1)*sizeof(char));
        sprintf(_index, "int@%d", index->value.intVal);
    }
    else if (index->type == IDENTIFIER) {
        generateUniqueIdentifier(index->lexeme, &uniq);
        len = snprintf(_index, 0, "LF@%s", uniq);
        _index = (char *) malloc((len+1)*sizeof(char));
        sprintf(_index, "LF@%s", uniq);
    }

    generateUniqueIdentifier(var->lexeme, &uniq);

    generateStrlen("GF", "tmp1", "LF", uniq);

    // strlen(var) = 0
    generateJumpifeq(ord_err, "GF", "tmp1", "int", "0");

    // index < strlen(var)
    printf("LT GF@valid %s %s\n", _index, "GF@tmp1");
    generateJumpifeq(ord_err, "GF", "valid", "bool", "false");

    // index < 0
    printf("LT GF@valid %s int@0\n", _index);
    generateJumpifeq(ord_err, "GF", "valid", "bool", "true");

    // Get char, push to stack and convert
    printf("GETCHAR GF@char LF@%s %s\n", uniq, _index);
    generatePushs("GF", "char");
    generateStrI2Ints();
    generateJump(ord_ret);

    // Error label
    generateLabel(ord_err);
    generatePushs("int", "0");

    // Return label
    generateLabel(ord_ret);
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
void callBIChr(T_TOKEN *var) {
    char *uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generatePushs("LF", uniq);
    generateInt2Chars();
    free(uniq);
}

/**
 * @brief Prints the provided variable to the output.
 * 
 * This function prints the provided variable to standard output.
 * 
 * @param var The variable to print
 */
void callBIWrite(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq = NULL;
        generateUniqueIdentifier(var->lexeme, &uniq);
        generateWrite("LF", uniq);
        free(uniq);
    }
    else if (var->type == INT) {
        printf("WRITE int@%d\n", var->value.intVal);
    }
    else if (var->type == FLOAT) {
        printf("WRITE float@%lf\n", var->value.floatVal);
    }
    else if (var->type == STRING) {
        char *out = NULL;
        handleCorrectStringFormat(var->value.stringVal, &out);
        generateWrite("string", out);
        free(out);
    }
}

/**
 * @brief Calls the built-in function based on the provided function call.
 * 
 * This function calls the built-in function based on the provided function call.
 * 
 * @param fn The function call to handle.
 */
void callBIFn(T_FN_CALL *fn) {
    if (strcmp(fn->name, "ifj.readstr") == 0) {
        callBIReadString();
    }
    else if (strcmp(fn->name, "ifj.readi32") == 0) {
        callBIReadInt();
    }
    else if (strcmp(fn->name, "ifj.readf64") == 0) {
        callBIReadFloat();
    }
    else if (strcmp(fn->name, "ifj.write") == 0) {
        callBIWrite(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.i2f") == 0) {
        callBIInt2Float(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.f2i") == 0) {
        callBIFloat2Int(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.string") == 0) {
        callBIString(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.length") == 0) {
        callBILength(fn->argv[0]);
    }
    else if (strcmp(fn->name, "ifj.concat") == 0) {
        callBIConcat(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.substring") == 0) {
        callBISubstring(fn->argv[0], fn->argv[1], fn->argv[2]);
    }
    else if (strcmp(fn->name, "ifj.strcmp") == 0) {
        callBIStrcmp(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.ord") == 0) {
        callBIOrd(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.chr") == 0) {
        callBIChr(fn->argv[0]);
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
 * accordingly to the labelElse if the expression is False.
 * 
 * @param labelElse The label to jump to.
 * 
 * @note The expression must be solved before calling this function.
 */
void handleIfStartBool(char *labelElse) {
    generatePops("GF", "tmp1");
    generateJumpifneq(labelElse, "GF", "tmp1", "bool", "true");
}

/**
 * @brief Handles the start of an if statement with a numerical expression.
 * 
 * This function handles the start of an if statement with a numerical expression, jumps
 * accordingly to the labelElse if the expression is NULL (end of loop).
 * 
 * @param labelElse The label to jump to.
 * @param var The variable to store the expression result.
 * 
 * @note The expression must be solved before calling this function.
 */
void handleIfStartNil(char *labelElse, T_TOKEN *var) {
    generatePops("GF", "tmp1");
    generateType("GF", "tmp2", "GF", "tmp1");
    generateJumpifeq(labelElse, "GF", "tmp2", "string", "nil");

    char *uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generateDefvar("LF", uniq);
    generateMove("LF", uniq, "GF", "tmp1");
    free(uniq);
}

/**
 * @brief Creates the else part of an if statement.
 * 
 * This function creates the else part of an if statement and jumps to the end of the if statement.
 * 
 * @param labelEnd The end label to jump to.
 * @param labelElse The else label to jump to.
 */
void createIfElse(char *labelEnd, char *labelElse) {
    generateJump(labelEnd);
    generateLabel(labelElse);
}

/**
 * @brief Creates the end of an if statement.
 * 
 * This function creates the end of an if statement.
 * 
 * @param labelEnd The end label to jump to.
 */
void createIfEnd(char *labelEnd) {
    generateLabel(labelEnd);
}

/**
 * @brief Creates the start of a while statement with a boolean expression.
 * 
 * This function creates the start of a while statement with a boolean expression.
 * 
 * @param labelStart The start label to jump to.
 */
void createWhileBoolHeader(char *labelStart, int upper, int current) { // Upper > -1, not inside of while, >= 0 inside of while (ID OF TOP WHILE)
    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@whileIsDefined$%d bool@true\n", labelCounter, upper);
        printf("DEFVAR LF@whileIsDefined$%d\n", current);
        printf("MOVE LF@whileIsDefined$%d bool@false\n", current);
        printf("LABEL skipDefvar$%d\n", labelCounter);
        labelCounter++;
    }
    else {
        printf("DEFVAR LF@whileIsDefined$%d\n", current);
        printf("MOVE LF@whileIsDefined$%d bool@false\n", current);
    }
    generateLabel(labelStart);
}

/**
 * @brief Creates the start of a while statement with a numerical expression.
 * 
 * This function creates the start of a while statement with a numerical expression.
 * 
 * @param labelStart The start label to jump to.
 * @param var The variable to store the expression result.
 */
void createWhileNilHeader(char *labelStart, T_TOKEN *var, int upper, int current) {
    if (upper >= 0) {
        printf("JUMPIFEQ skipDefvar$%d LF@whileIsDefined$%d bool@true\n", labelCounter, upper);
        printf("DEFVAR LF@whileIsDefined$%d\n", current);
        printf("MOVE LF@whileIsDefined$%d bool@false\n", current);
        printf("LABEL skipDefvar$%d\n", labelCounter);
        labelCounter++;
    }
    else {
        printf("DEFVAR LF@whileIsDefined$%d\n", current);
        printf("MOVE LF@whileIsDefined$%d bool@false\n", current);
    }

    char *uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generateDefvar("LF", uniq);
    free(uniq);

    generateLabel(labelStart);
}

/**
 * @brief Handles the while statement with a boolean expression.
 * 
 * This function handles the while statement with a boolean expression.
 * 
 * @param labelEnd The end label to jump to.
 */
void handleWhileBool(char *labelEnd) {
    generatePops("GF", "tmp1");
    generateJumpifneq(labelEnd, "GF", "tmp1", "bool", "true");
}

/**
 * @brief Handles the while statement with a numerical expression.
 * 
 * This function handles the while statement with a numerical expression.
 * 
 * @param labelEnd The end label to jump to.
 * @param var The variable to store the expression result.
 */
void handleWhileNil(char *labelEnd, T_TOKEN *var) {
    generatePops("GF", "tmp1");
    generateType("GF", "tmp2", "GF", "tmp1");
    generateJumpifeq(labelEnd, "GF", "tmp2", "string", "nil");

    char *uniq = NULL;
    generateUniqueIdentifier(var->lexeme, &uniq);
    generateMove("LF", uniq, "GF", "tmp1");
    free(uniq);
}

/**
 * @brief Creates the end of a while statement.
 * 
 * This function creates the end of a while statement.
 * 
 * @param labelStart The start label to jump to.
 * @param labelEnd The end label to jump to.
 */
void createWhileEnd(char *labelStart, char *labelEnd, int whileDefCounter) {
    printf("MOVE LF@whileIsDefined$%d bool@true\n", whileDefCounter);
    generateJump(labelStart);
    generateLabel(labelEnd);
}