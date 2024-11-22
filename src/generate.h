// FILE: generator.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)s
// AUTHORS:
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES: Header file for the code generation instruction set for the IFJ24 language compiler.

#ifndef GENERATE_H
#define GENERATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void generateHeader();
void generateMove(char *frame, char *var, char *_frame, char *_var);
void generateCreateFrame();
void generatePushFrame();
void generatePopFrame();
void generateDefvar(char *frame, char *var);
void generateCall(char *label);
void generateReturn();
void generatePushs(char *frame, char *var);
void generatePushsInt(int var);
void generatePushsFloat(float var);
void generatePushsString(char *var);
void generatePops(char *frame, char *var);
void generateClears();
void generateAdd(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateSub(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateMul(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateDiv(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateIdiv(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateAdds();
void generateSubs();
void generateMuls();
void generateDivs();
void generateIdivs();
void generateLt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateGt(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateEq(char *frame, char *var, char *_frame, char *_var, char *__frame, char *__var);
void generateLts();
void generateGts();
void generateEqs();
void generateAnd(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generateOr(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generateNot(char *frame, char *var, char *_frame, char *_symb);
void generateAnds();
void generateOrs();
void generateNots();
void generateInt2Float(char *frame, char *var, char *_frame, char *_symb);
void generateFloat2Int(char *frame, char *var, char *_frame, char *_symb);
void generateInt2Char(char *frame, char *var, char *_frame, char *_symb);
void generateStrI2Int(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generateInt2Floats();
void generateFloat2Ints();
void generateInt2Chars();
void generateStrI2Ints();
void generateRead(char *frame, char *var, char *type);
void generateWrite(char *frame, char *var);
void generateConcat(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generateStrlen(char *frame, char *var, char *_frame, char *_symb);
void generateGetchar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generateSetchaar(char *frame, char *var, char *_frame, char *_symb, char *__frame, char *__symb);
void generateType(char *frame, char *var, char *_frame, char *_symb);
void generateLabel(char *label);
void generateJump(char *label);
void generateJumpifeq(char *label, char *frame, char *symb, char *_frame, char *_symb);
void generateJumpifneq(char *label, char *frame, char *symb, char *_frame, char *_symb);
void generateJumpifeqs(char *label);
void generateJumpifneqs(char *label);
void generateExit(int *symb);
void generateBreak();
void generateDprint(char *frame, char *symb);

#endif // GENERATE_H
