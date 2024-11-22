
// FILE: gen_handler.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  <Kryštof Valenta> (xvalenk00)
//
// YEAR: 2024
// NOTES: Header file for gen_handler.c

#ifndef GEN_HANDLER_H
#define GEN_HANDLER_H

#include "generate.h"
#include "precedence_tree.h"
#include "symtable.h"

// Function declarations
void generateUniqueIdentifier(char *name, char *uniq_name);
void createFnHeader(char *name, T_TOKEN **args, int argCount);
void createProgramHeader();
void callFunction(char *name, T_TOKEN **args, int argCount);
void createReturn();
void handleDiscard();
void handleUniqDefvar(T_TOKEN *var);
void createStackByPostorder(T_TREE_NODE *tree);
void handleAssign(char *var);
void callBIReadInt();
void callBIReadFloat();
void callBIReadString();
void callBIInt2Float(T_TOKEN *var);
void callBIFloat2Int(T_TOKEN *var);
void callBIString(T_TOKEN *var);
void callBILength(T_TOKEN *var);
void callBIConcat(T_TOKEN *var, T_TOKEN *_var);
void callBISubstring(T_TOKEN *var, T_TOKEN *beg, T_TOKEN *end);
void callBIStrcmp(T_TOKEN *var, T_TOKEN *_var);
void callBIOrd(T_TOKEN *var, T_TOKEN *index);
void callBIChr(T_TOKEN *var);
void handleIfStartBool(char *labelElse);
void handleIfStartNil(char *labelElse, T_TOKEN *var);
void createIfElse(char *labelEnd, char *labelElse);
void createIfEnd(char *labelEnd);
void createWhileBoolHeader(char *labelStart);
void createWhileNilHeader(char *labelStart, T_TOKEN *var);
void handleWhileBool(char *labelEnd);
void handleWhileNil(char *labelEnd, T_TOKEN *var);
void createWhileEnd(char *labelStart, char *labelEnd);

#endif // GEN_HANDLER_H