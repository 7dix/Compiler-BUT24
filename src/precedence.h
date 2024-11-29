// FILE: precedence.h
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Header file for precedence syntax analysis of expressions


#ifndef H_PRECEDENCE
#define H_PRECEDENCE

#include "precedence_stack.h"

// Declaration of precedences of precedence table
typedef enum PRECEDENCE{
  GR_COMP,    // >
  LSS_COMP,   // <
  EQ_COMP,    // =
  ERR=-1,     // error
} PRECEDENCE;

// Declaration of indexes of operators in precedence table
typedef enum OPERATOR_INDEX{

    ID,           // Identifer 
    PL,           // +
    MIN,          // -
    MUL,          // *
    DIV,          // /
    LES,          // <
    GRT,          // >
    LESS_EQ,      // <=
    GRT_EQ,       // >=
    EQ,           // ==
    NEQ,          // !=
    LPAR,         // (
    RPAR,         // )
    DOLLAR,       // $
    ERR_INDEX=-1, // Error index

} OPERATOR_INDEX;

// Declaration of types of end of expression
typedef enum TYPE_END{
  IF_WHILE_END,     
  ASS_END,   
} TYPE_END;

// Function declarations for return index of the operator in precedence table
OPERATOR_INDEX priorityIndexTable(TOKEN_TYPE type);

// Function declarations for return precedence in table
PRECEDENCE getPrecedence(OPERATOR_INDEX row, OPERATOR_INDEX coll);

// Function declarations for main function of precedence syntax analysis
T_RET_VAL precedenceSyntaxMain(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree, TYPE_END typeEnd);


// Function declaration for search reuce rule
int canReduce(T_STACK_PTR stack);

// Function declaritions or reduce
bool reduce(T_STACK_PTR stack, T_TREE_NODE_PTR *tree, int rule, bool makeTree);


#endif // H_PRECEDENCE