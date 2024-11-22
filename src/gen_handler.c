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
#include "precedence_tree.h"
#include "symtable.h"
#include "semantic.h"

// call built-in function base on fn name
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
        callBISubstr(fn->argv[0], fn->argv[1], fn->argv[2]);
    }
    else if (strcmp(fn->name, "ifj.strcmp") == 0) {
        callBIStrcmp(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.ord") == 0) {
        callBIFind(fn->argv[0], fn->argv[1]);
    }
    else if (strcmp(fn->name, "ifj.chr") == 0) {
        callBISort(fn->argv[0]);
    }
}

void generateUniqueIdentifier(char *name, char *uniq_name) {
    sprintf(uniq_name, "%s$%d", name, get_var_id(ST, name));
}



// Function to create function header
void createFnHeader(char *name, T_TOKEN **args, int argCount) {
    generateLabel(name);
    generateCreateFrame();
    generatePushFrame();
    for (int i = argCount; i > 0; i--) {
        char *uniq;
        generateUniqueIdentifier(args[i]->lexeme, uniq);
        generateDefvar("LF", uniq);
        generatePops("LF", uniq);
    }
}

// Function to create write
void callBIWrite(T_TOKEN *var) {
    if (var->type == IDENTIFIER) {
        char *uniq;
        generateUniqueIdentifier(var->lexeme, uniq);
        generateWrite("LF", uniq);
    }
    else if (var->type == INT) {
        printf("WRITE int@%d\n", var->value.intVal);
    }
    else if (var->type == FLOAT) {
        printf("WRITE float@%lf\n", var->value.intVal);
    }
    else if (var->type == STRING) {
        generateWrite("string", var->value.stringVal);
    }
}

// Function to create program header
void createProgramHeader() {
    generateHeader();
    generateDefvar("GF", "tmp1");
    generateDefvar("GF", "tmp2");
    generateDefvar("GF", "char");
    generateDefvar("GF", "char2");
    generateDefvar("GF", "valid");
    generateDefvar("GF", "trash");
    generateDefvar("GF", "beg");
    generateCall("main");
    generateExit(0);
}

void callFunction(char *name, T_TOKEN **args, int argCount) {
    for (int i = 0; i < argCount; i++) {
        if (args[i]->type == IDENTIFIER) {
            char *uniq;
            generateUniqueIdentifier(args[i]->lexeme, uniq);
            generatePushsChar("LF", uniq);
        }
        else if (args[i]->type == INT) {
            generatePushsInt(args[i]->value.intVal);
        }
        else if (args[i]->type == FLOAT) {
            generatePushsFloat(args[i]->value.floatVal);
        }
        else if (args[i]->type == STRING) {
            generatePushsString(args[i]->value.stringVal);
        }
    }

    generateCall(name);
}

// Function to create return statement
void createReturn() {
    generatePopFrame();
    generateReturn();
}

void handleDiscard() {
    generatePops("GF", "trash");
}

void handleUniqDefvar(T_TOKEN *var) {
    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    generateDefvar("LF", uniq);
}

// Function to solve expression via stack by postorder traversal
void createStackByPostorder(T_TREE_NODE *tree) {

    if (tree == NULL) return; // Empty tree

    createStackByPostorder(tree->left);
    createStackByPostorder(tree->right);

    if (tree->token->type == IDENTIFIER) {
        char *uniq;
        generateUniqueIdentifier(tree->token->lexeme, uniq);
        generatePushs("LF", uniq);

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
    // NOTE: Is needed?
    // else if (tree->token->type == STRING) {
    //     generatePushsString(tree->token->value.stringVal);
    // }
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

void handleAssign(char *var) {
    char *uniq;
    generateUniqueIdentifier(var, uniq);
    generatePops("LF", uniq);
}

// Function to handle built-in read int
void callBIReadInt() {
    generateRead("GF", "tmp1", "int");
    generatePushs("GF", "tmp1");
}

// Function to handle built-in read float
void callBIReadFloat() {
    generateRead("GF", "tmp1", "float");
    generatePushs("GF", "tmp1");
}

// Function to handle built-in read string
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
        char *uniq;
        generateUniqueIdentifier(var->lexeme, uniq);
        generatePushs("LF", uniq);
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
        char *uniq;
        generateUniqueIdentifier(var->lexeme, uniq);
        generatePushs("LF", uniq);
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
        char *uniq;
        generateUniqueIdentifier(var->lexeme, uniq);
        generatePushs("LF", uniq);
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
    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    generateStrlen("GF", "tmp1", "LF", uniq);
    generatePushs("GF", "tmp1");
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
    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    char *_uniq;
    generateUniqueIdentifier(_var->lexeme, _uniq);
    generateConcat("GF", "tmp1", "LF", uniq, "LF", _uniq);
    generatePushs("GF", "tmp1");
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

    char *_beg, *_end;
    char *uniq_beg, *uniq_end;

    if (beg->type == INT) {
        sprintf(_beg, "int@%d", beg->value.intVal);
    }
    else if (beg->type == IDENTIFIER) {
        generateUniqueIdentifier(beg->lexeme, uniq_beg);
        sprintf(_beg, "LF@%s", uniq_beg);
    }

    if (end->type == INT) {
        sprintf(_end, "int@%d", end->value.intVal);
    }
    else if (end->type == IDENTIFIER) {
        generateUniqueIdentifier(end->lexeme, uniq_end);
        sprintf(_end, "LF@%s", uniq_end);
    }

    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);

    generateStrlen("GF", "tmp1", "LF", uniq);

    // beg < 0
    printf("LT GF@valid int@0 %s\n", _beg);
    generateJumpifeq("substr_err", "GF", "valid", "bool", "true");

    // end < 0
    printf("LT GF@valid int@0 %s\n", _end);
    generateJumpifeq("substr_err", "GF", "valid", "bool", "true");

    // beg > end
    printf("GT GF@valid %s %s\n", _beg, _end);
    generateJumpifeq("substr_err", "GF", "valid", "bool", "true");

    // beg >= length(var)
    printf("GT GF@valid %s GF@tmp1\n", _beg);
    printf("EQ GF@tmp2 %s GF@tmp1\n", _beg);
    printf("OR GF@valid GF@valid GF@tmp2\n");
    generateJumpifeq("substr_err", "GF", "valid", "bool", "true");

    // end > length(var)
    printf("GT GF@valid %s GF@tmp1\n", _end);
    generateJumpifeq("substr_err", "GF", "valid", "bool", "true");

    // move "string@" to dest
    printf("MOVE GF@tmp2 string@\n");

    // Loop
    generateLabel("substr_loop");
    printf("LT GF@valid %s %s\n", _beg, _end);
    generateJumpifeq("substr_end", "GF", "valid", "bool", "false");

    // get char
    printf("GETCHAR GF@char LF@%s %s\n", uniq, _beg);
    generateConcat("GF", "tmp2", "GF", "tmp2", "GF", "char");

    // increment beg
    printf("ADD GF@beg %s int@1\n", _beg);
    generateJump("substr_loop");

    // End of loop
    generateLabel("substr_end");
    generatePushs("GF", "tmp2");
    generateJump("substr_ret");

    // Error handling
    generateLabel("substr_err");
    generateMove("GF", "tmp2", "nil", "nil");
    generatePushs("GF", "tmp2");
    generateJump("substr_ret");

    // Return label
    generateLabel("substr_ret");

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
    char *uniq, *_uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    generateUniqueIdentifier(_var->lexeme, _uniq);

    // TODO: CHECK SEMANTIC CORRECTNESS
    generateDefvar("LF", "$0index"); // Char index
    generateMove("LF", "$0index", "int", 0); // Initialize index to 0

    // Loop start
    generateLabel("strcmp_loop");

    // Save the length of both strings
    generateStrlen("GF", "tmp1", "LF", uniq);
    generateStrlen("GF", "tmp2", "LF", _uniq);

    // If index is greater+1 than the length of the string, evaluate and return
    generateLt("GF", "valid", "LF", "$0index", "GF", "tmp1"); // index < strlen(var1)
    generateJumpifeq("strcmp_end_length", "GF", "valid", "bool", "false");
    generateLt("GF", "valid", "LF", "$0index", "GF", "tmp2"); // index < strlen(var2)
    generateJumpifeq("strcmp_end_length", "GF", "valid", "bool", "false");

    // Get the characters at the index
    generateGetchar("GF", "char", "LF", uniq, "LF", "$0index");
    generateGetchar("GF", "char2", "LF", _uniq, "LF", "$0index");

    // Compare the characters, if not equal, return
    generateEq("GF", "valid", "GF", "char", "GF", "char2");
    generateJumpifeq("strcmp_end", "GF", "valid", "bool", "false");

    // Increment the index and loop
    generateAdd("LF", "$0index", "LF", "$0index", "int", "1");
    generateJump("strcmp_loop");

    // Handle the end of loop by length
    generateLabel("strcmp_end_length");
    generateLt("GF", "valid", "GF", "tmp1", "GF", "tmp2"); // strlen(var1) < strlen(var2)
    generateJumpifeq("strcmp_ret_lesser", "GF", "valid", "bool", "true");
    generateGt("GF", "valid", "GF", "tmp1", "GF", "tmp2"); // strlen(var1) > strlen(var2)
    generateJumpifeq("strcmp_ret_greater", "GF", "valid", "bool", "true");

    // Strings are equal
    generateLabel("strcmp_end");
    generatePushs("int", "0");
    generateJump("strcmp_ret");

    // S1 lesser
    generateLabel("strcmp_ret_lesser");
    generatePushs("int", "-1");
    generateJump("strcmp_ret");

    // S1 greater
    generateLabel("strcmp_ret_greater");
    generatePushs("int", "1");
    generateJump("strcmp_ret");

    // Return label
    generateLabel("strcmp_ret");
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
    char *uniq;
    char *_index;

    if (index->type == INT) {
        sprintf(_index, "int@%d", index->value.intVal);
    }
    else if (index->type == IDENTIFIER) {
        generateUniqueIdentifier(index->lexeme, uniq);
        sprintf(_index, "LF@%s", uniq);
    }

    generateUniqueIdentifier(var->lexeme, uniq);

    generateStrlen("GF", "tmp1", "LF", uniq);

    // strlen(var) = 0
    generateJumpifeq("ord_err", "GF", "tmp1", "int", "0");

    // index < strlen(var)
    printf("LT GF@valid %s %s\n", _index, "GF@tmp1");
    generateJumpifeq("ord_err", "GF", "valid", "bool", "false");

    // index < 0
    printf("LT GF@valid %s int@0\n", _index);
    generateJumpifeq("ord_err", "GF", "valid", "bool", "true");

    // Get char, push to stack and convert
    generateGetchar("GF", "char", "LF", uniq, "LF", _index);
    generatePushs("GF", "char");
    generateStrI2Ints();
    generateJump("ord_ret");

    // Error label
    generateLabel("ord_err");
    generatePushs("int", "0");

    // Return label
    generateLabel("ord_ret");
}

void callBIChr(T_TOKEN *var) {
    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    generatePushs("LF", uniq);
    generateInt2Chars();
}

/***********************************************************************
 *                         CONDITIONS & LOOPS
 ***********************************************************************
 * Functions to handle conditions and loops
 * Before calling each, both the label and expression
 * solution must be generated.
 */

// Function to handle if start with boolean
void handleIfStartBool(char *labelElse) {
    generatePops("GF", "tmp1");
    generateJumpifneq(labelElse, "GF", "tmp1", "bool", "true");
}

// Function to handle if start with nil
void handleIfStartNil(char *labelElse, T_TOKEN *var) {
    generatePops("GF", "tmp1");
    generateType("GF", "tmp2", "GF", "tmp1");
    generateJumpifeq(labelElse, "GF", "tmp2", "nil", "nil");

    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    generateMove("LF", uniq, "GF", "tmp1");
}

// Function to create if else
void createIfElse(char *labelEnd, char *labelElse) {
    generateJump(labelEnd);
    generateLabel(labelElse);
}

// Function to create if end
void createIfEnd(char *labelEnd) {
    generateLabel(labelEnd);
}

// Function to create while header with boolean
void createWhileBoolHeader(char *labelStart) {
    generateLabel(labelStart);
}

// Function to create while header with nil
void createWhileNilHeader(char *labelStart, T_TOKEN *var) {
    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    generateDefvar("LF", uniq);

    generateLabel(labelStart);
}

// Function to handle while with boolean
void handleWhileBool(char *labelEnd) {
    generatePops("GF", "tmp1");
    generateJumpifneq(labelEnd, "GF", "tmp1", "bool", "true");
}

// Function to handle while with nil
void handleWhileNil(char *labelEnd, T_TOKEN *var) {
    generatePops("GF", "tmp1");
    generateType("GF", "tmp2", "GF", "tmp1");
    generateJumpifeq(labelEnd, "GF", "tmp2", "nil", "nil");

    char *uniq;
    generateUniqueIdentifier(var->lexeme, uniq);
    generateMove("LF", uniq, "GF", "tmp1");
}

// Function to create while end
void createWhileEnd(char *labelStart, char *labelEnd) {
    generateJump(labelStart);
    generateLabel(labelEnd);
}