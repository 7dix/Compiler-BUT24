// FILE: generator.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)s
// AUTHORS:
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES: Code generation instruction set for the IFJ24 language compiler.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


/***********************************************************************
 *                  CONVERSION TO PRINTABLE STRING
 ***********************************************************************
*/
 
void handleCorrectStringFormat(char *input, char **output) {
    *output = (char *)malloc(4 * strlen(input) + 1); // Allocate enough memory for the worst case
    *output[0] = '\0'; // Init the output string

    while (*input) {
        char current = *input;
        
        if (current == 35 || current == 92 || current <= 32) {
            char hex[6];

            if (current == '\n') { // Special newline case
                sprintf(hex, "\\010");
            }
            else if (current == '\t') { // Special tab case
                sprintf(hex, "\\009");
            }
            else if (current == '\r') {// Special carriage return case
                sprintf(hex, "\\013");
            }
            else {
                sprintf(hex, "\\%03d", current); // Other cases
                strcat(*output, hex);
            }
        }
        else {
            size_t len = strlen(*output);
            (*output)[len] = current;
            (*output)[len + 1] = '\0'; // Always null-terminate the string
        }
        input++;
    }
}

/***********************************************************************
 *                  INSTRUCTION GENERATIVE FUNCTIONS
 ***********************************************************************
 * Functions that generate the IFJ24 language instructions to call in the
 * interpreter.
*/

void generateHeader() {
    printf(".IFJcode24\n");
}

void generateMove(char *frame, char *var, char *_frame, char *_var) {
    printf("MOVE %s@%s %s@%s\n", frame, var, _frame, _var);
}

void generateCreateFrame() {
    printf("CREATEFRAME\n");
}

void generatePushFrame() {
    printf("PUSHFRAME\n");
}

void generatePopFrame() {
    printf("POPFRAME\n");
}

void generateDefvar(char *frame, char *var) {
    printf("DEFVAR %s@%s\n", frame, var);
}

void generateCall(char *label) {
    printf("CALL %s\n", label);
}

void generateReturn() {
    printf("RETURN\n");
}

void generatePushs(char *frame, char *var) {
    printf("PUSHS %s@%s\n", frame, var);
}

void generatePushsInt(int var) {
    printf("PUSHS int@%d\n", var);
}

void generatePushsFloat(float var) {
    printf("PUSHS float@%a\n", var);
}

void generatePushsString(char *var) {
    char *out = NULL;
    handleCorrectStringFormat(var, &out);
    printf("PUSHS string@%s\n", out);
    free(out);
}

void generatePops(char *frame, char *var) {
    printf("POPS %s@%s\n", frame, var);
}

void generateClears() {
    printf("CLEARS\n");
}

void generateAdd(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("ADD %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateSub(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("SUB %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateMul(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("MUL %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateDiv(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("DIV %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateIdiv(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("IDIV %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateAdds() {
    printf("ADDS\n");
}

void generateSubs() {
    printf("SUBS\n");
}

void generateMuls() {
    printf("MULS\n");
}

void generateDivs() {
    printf("DIVS\n");
}

void generateIdivs() {
    printf("IDIVS\n");
}

void generateLt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("LT %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateGt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("GT %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateEq(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var) {
    printf("EQ %s@%s %s@%s %s@%s\n", frame, var, _frame, _var, __frame, __var);
}

void generateLts() {
    printf("LTS\n");
}

void generateGts() {
    printf("GTS\n");
}

void generateEqs() {
    printf("EQS\n");
}

void generateAnd(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("AND %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generateOr(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("OR %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generateNot(char *frame, char *var, char *_frame, char *_symb) {
    printf("NOT %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generateAnds() {
    printf("ANDS\n");
}

void generateOrs() {
    printf("ORS\n");
}

void generateNots() {
    printf("NOTS\n");
}

void generateInt2Float(char *frame, char *var, char *_frame, char *_symb) {
    printf("INT2FLOAT %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generateFloat2Int(char *frame, char *var, char *_frame, char *_symb) {
    printf("FLOAT2INT %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generateInt2Char(char *frame, char *var, char *_frame, char *_symb) {
    printf("INT2CHAR %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generateStrI2Int(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("STRI2INT %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generateInt2Floats() {
    printf("INT2FLOATS\n");
}

void generateFloat2Ints() {
    printf("FLOAT2INTS\n");
}

void generateInt2Chars() {
    printf("INT2CHARS\n");
}

void generateStrI2Ints() {
    printf("STRI2INTS\n");
}

void generateRead(char *frame, char *var, char *type) {
    if (strcmp(type, "int") == 0) {
        printf("READ %s@%s int\n", frame, var);
    }

    else if (strcmp(type, "float") == 0) {
        printf("READ %s@%s float\n", frame, var);
    }

    else if (strcmp(type, "string") == 0) {
        printf("READ %s@%s string\n", frame, var);
    }

    else if (strcmp(type, "bool") == 0) {
        printf("READ %s@%s bool\n", frame, var);
    }

    else{
        exit(99);
    }
}

void generateWrite(char *frame, char *var) {
    printf("WRITE %s@%s\n", frame, var);
}

void generateConcat(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("CONCAT %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generateStrlen(char *frame, char *var, char *_frame, char *_symb) {
    printf("STRLEN %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generateGetchar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("GETCHAR %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generateSetchaar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb) {
    printf("SETCHAR %s@%s %s@%s %s@%s\n", frame, var, _frame, _symb, __frame, __symb);
}

void generateType(char *frame, char *var, char *_frame, char *_symb) {
    printf("TYPE %s@%s %s@%s\n", frame, var, _frame, _symb);
}

void generateLabel(char *label) {
    printf("LABEL %s\n", label);
}

void generateJump(char *label) {
    printf("JUMP %s\n", label);
}

void generateJumpifeq(char *label, char *frame, char *symb, char *_frame, char *_symb) {
    printf("JUMPIFEQ %s %s@%s %s@%s\n", label, frame, symb, _frame, _symb);
}

void generateJumpifneq(char *label, char *frame, char *symb, char *_frame, char *_symb) {
    printf("JUMPIFNEQ %s %s@%s %s@%s\n", label, frame, symb, _frame, _symb);
}

void generateJumpifeqs(char *label) {
    printf("JUMPIFEQS %s\n", label);
}

void generateJumpifneqs(char *label) {
    printf("JUMPIFNEQS %s\n", label);
}

void generateExit(int symb) {
    printf("EXIT int@%d\n", symb);
}

void generateBreak() {
    printf("BREAK\n");
}

void generateDprint(char *frame, char *symb) {
    printf("DPRINT %s@%s\n", frame, symb);
}

/** End of the instruction generative functions */