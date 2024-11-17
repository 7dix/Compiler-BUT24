// FILE: precedence.c
// PROJECT: IFJ24 - Compiler for the IFJ24 language @ FIT BUT 2BIT
// TEAM: Martin Zůbek (253206)
// AUTHORS:
//  Martin Zůbek (253206)
//
// YEAR: 2024
// NOTES: Implementation of precedence syntax analysis of expressions



#include "precedence.h"


/**
 * GRAMAR:
 * 
 * S -> R
 * 
 * R -> T
 * R -> E
 * 
 * E -> E + E
 * E -> E - E
 * E -> E*E
 * E -> E/E
 * E ->(E)
 * E -> id
 * E -> int_value
 * E -> float_value
 * 
 * T -> E < E
 * T -> E > E
 * T -> E <= E
 * T -> E >= E
 * T -> E == E
 * T -> E != E 
 * T -> E
 * 
 */



// Precednce table 
PRECEDENCE precedenceTable[14][14] = {
    //              ID        +         -         *         /          <         >         <=         >=        ==         !=         (           )         $
    /* ID */   { GR_COMP,  GR_COMP,  GR_COMP,  GR_COMP,  GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   ERR,       GR_COMP,  GR_COMP },
    /* +  */   { LSS_COMP, GR_COMP,  GR_COMP,  LSS_COMP, LSS_COMP,  GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
    /* -  */   { LSS_COMP, GR_COMP,  GR_COMP,  LSS_COMP, LSS_COMP,  GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
    /* *  */   { LSS_COMP, LSS_COMP, GR_COMP,  GR_COMP,  GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
    /* /  */   { LSS_COMP, LSS_COMP, GR_COMP,  GR_COMP,  GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
    /* <  */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  ERR,      ERR,       ERR,       ERR,      ERR,       ERR,       LSS_COMP,  GR_COMP,  GR_COMP },
    /* >  */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  ERR,      ERR,       ERR,       ERR,      ERR,       ERR,       LSS_COMP,  GR_COMP,  GR_COMP },
    /* <= */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  ERR,      ERR,       ERR,       ERR,      ERR,       ERR,       LSS_COMP,  GR_COMP,  GR_COMP },
    /* >= */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  ERR,      ERR,       ERR,       ERR,      ERR,       ERR,       LSS_COMP,  GR_COMP,  GR_COMP },
    /* == */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  ERR,      ERR,       ERR,       ERR,      ERR,       ERR,       LSS_COMP,  GR_COMP,  GR_COMP },
    /* != */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  ERR,      ERR,       ERR,       ERR,      ERR,       ERR,       LSS_COMP,  GR_COMP,  GR_COMP },
    /* (  */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  LSS_COMP, LSS_COMP,  LSS_COMP,  LSS_COMP, LSS_COMP,  LSS_COMP,  LSS_COMP,  EQ_COMP,  ERR     },
    /* )  */   { ERR,      GR_COMP,  GR_COMP,  GR_COMP,  GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   ERR,       GR_COMP,  GR_COMP },
    /* $  */   { LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP, LSS_COMP,  LSS_COMP, LSS_COMP,  LSS_COMP, LSS_COMP,  LSS_COMP,  LSS_COMP,  LSS_COMP,  ERR,      ERR     }
};


/**
 * @brief Function for return index of the operator in precedence table
 * @param type Type of operator/token
 * @return Index of operator in precedence table, -1 if error
*/
OPERATOR_INDEX precedenceIndex(TOKEN_TYPE type){

    switch (type){
        case IDENTIFIER:return ID;
        case TYPE_INT:return ID;
        case TYPE_FLOAT:return ID;
        case PLUS:return PL;
        case MINUS:return MIN;
        case MULTIPLY:return MUL;
        case DIVIDE:return DIV;
        case LESS_THAN:return LES;
        case GREATER_THAN:return GRT;
        case LESS_THAN_EQUAL:return LESS_EQ;
        case GREATER_THAN_EQUAL:return GRT_EQ;
        case EQUAL:return EQ;
        case NOT_EQUAL:return NEQ;
        case BRACKET_LEFT_SIMPLE:return LPAR;
        case BRACKET_RIGHT_SIMPLE:return RPAR;
        default: return ERR_INDEX;
    }
}

/**
 * @brief Function for return precedence in table, maps row and collumn to precedence table
 * @param row  Index of row in precedence table
 * @param coll Index of collumn in precedence table
 * @return Precdence in table, -1 if error
*/
PRECEDENCE getPrecedence(OPERATOR_INDEX row, OPERATOR_INDEX coll){ 
    if(row != -1 && coll != -1) return precedenceTable[row][coll];
    else return ERR;
}


/**
 * @brief Function for freed memory of stack and tree
 * @param stack Pointer on stack for freed memory
 * @param tree Pointer on tree for freed memory 
*/
void destroy_all(T_STACK_PTR stack, T_TREE_NODE_PTR *tree){
    tree_dispose(tree);
    stack_dispose(stack);
    return;
}

int reduceVaribles(T_STACK_PTR stack){
    int countOfVarRed = 0;
    for ( T_STACK_ITEM_PTR top = stack_top(stack); top != NULL || top->type != SHIFT; top = top->prev){
        countOfVarRed++;
    }
    return countOfVarRed;
    
}


int canReduce(T_STACK_PTR stack){

    int countOfVarRed = reduceVaribles(stack);
    // Rule doesn't exist
    if(countOfVarRed != 1 || countOfVarRed != 3) return 0;

    // E -> id | int_value | float_value
    if (countOfVarRed == 1 && stack->top->type == TERMINAL) return 1;
    // E -> R | T
    if (countOfVarRed == 1 && (stack->top->type == NON_TERMINAL_R || stack->top->type == NON_TERMINAL_E )) return 2;
    
    T_STACK_ITEM_PTR top = stack_top(stack);
    T_STACK_ITEM_PTR secondTop = top->prev;
    T_STACK_ITEM_PTR thirdTop = secondTop->prev;
    // E -> E + E | E - E | E * E | E / E
    if(countOfVarRed == 3 && (top->type == NON_TERMINAL_E || thirdTop->type == NON_TERMINAL_E)){
        switch (secondTop->token->type){
            case PLUS: return 3;
            case MINUS: return 4;
            case MULTIPLY: return 5;
            case DIVIDE: return 6;
            default: return 0;
        }
    }

    // T -> E < E | E > E | E <= E | E >= E | E == E | E != E
    if (countOfVarRed == 3 && (top->type == NON_TERMINAL_E || thirdTop->type == NON_TERMINAL_E)){
        switch (secondTop->token->type){
            case LESS_THAN: return 7;
            case GREATER_THAN: return 8;
            case LESS_THAN_EQUAL: return 9;
            case GREATER_THAN_EQUAL: return 10;
            case EQUAL: return 11;
            case NOT_EQUAL: return 12;
            default: return 0;
        }
    }    

    // E -> (E)
    if (countOfVarRed == 3 && top->type == L_B && (secondTop->type == NON_TERMINAL_E || secondTop->type == NON_TERMINAL_R) && thirdTop->type == R_B) return 13;

    return 0;


}

bool reduce(T_STACK_PTR stack, T_TREE_NODE_PTR *tree, int rule){
    T_STACK_ITEM_PTR top = stack_top(stack);
    switch (rule){
    case 1:
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);

        if(stack_push(stack, NULL, NON_TERMINAL_E)){
            return false;
        }
        break;
    case 2:

        stack_pop(stack);
        
        break;
    default: break;
    }

    if (rule == 3 || rule == 4 || rule == 5 || rule == 6){
        T_STACK_ITEM_PTR secondTop = top->prev;
        T_STACK_ITEM_PTR thirdTop = secondTop->prev;

        T_TREE_NODE_PTR root = createSubTree(tree, secondTop->node, top->node, thirdTop->node);

        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);

        if(stack_push(stack, NULL, NON_TERMINAL_E)){
            return false;
        }

        stack->top->node = root;
        
    }
    
    if (rule == 7 || rule == 8 || rule == 9 || rule == 10 || rule == 11 || rule == 12){
        T_STACK_ITEM_PTR secondTop = top->prev;
        T_STACK_ITEM_PTR thirdTop = secondTop->prev;

        T_TREE_NODE_PTR root = createSubTree(tree, secondTop->node,top->node, thirdTop->node);

        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);

        if(stack_push(stack, NULL, NON_TERMINAL_R)){
            return false;
        }

        stack->top->node = root;
    }


    if(rule == 13){
        
    
        T_STACK_ITEM_PTR oneNeterminal = stack_top(stack)->prev;
        STACK_ITEM_TYPE type = stack_top(stack)->type;
        
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);
        stack_pop(stack);

        if(type == NON_TERMINAL_E){
            if(stack_push(stack, NULL, NON_TERMINAL_E)){
                return false;
            }
        }

        if (type == NON_TERMINAL_R){
            if(stack_push(stack, NULL, NON_TERMINAL_R)){
                return false;
            }
        }

        stack->top->node = oneNeterminal->node;

    }






    return true;
}
/**
 * @brief Main function for precedence syntax analysis
 * @param buffer Pointer on buffer of tokens
 * @param tree Pointer on tree of expression, if analysis is successful
 * @param typeEnd Type of end of expression
 * @return 0 if analysis is successful, 2 if syntax error, 99 if internal error (malloc for example)
*/
RetVal precedenceSyntaxMain(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree, TYPE_END typeEnd){

    // Create and init stack
    T_STACK stack;
    stack_init(&stack);
    
    // Count of brackets
    int countBrac = 0;
    // Count of relational operators
    unsigned int countRelOperatoes = 0;

    // Flag for begin of expression
    bool beginDollar = true;
    // Flag for end of expression
    bool notEndDollar = false;

    bool continueReduction = false;    

    // Token for analysis
    T_TOKEN *token = NULL;

    // Precedence of operators
    PRECEDENCE precedence = ERR;

    
    // Precednce analysis
    while(is_empty(&stack) && beginDollar){

        // Get token from buffer, until end of expression
        if(!continueReduction && (notEndDollar || beginDollar)) next_token(buffer, &token);

        // Get the toppest terminal on stack
        T_STACK_ITEM_PTR topTerminal = stack_top_terminal(&stack);




        // Search for end of expression
        if(!continueReduction && (notEndDollar || beginDollar) && ((token->type == BRACKET_RIGHT_SIMPLE && countBrac == 0 && countRelOperatoes <= 1 && typeEnd == IF_WHILE_END) || (token->type == SEMICOLON && countBrac == 0 && countRelOperatoes <= 1 && typeEnd == ASS_END))) notEndDollar = false;

        // Counting relational operators in an expression  
        if (!continueReduction && (notEndDollar || beginDollar) && (token->type == EQUAL || token->type == NOT_EQUAL || token->type == LESS_THAN || token->type == LESS_THAN_EQUAL || token->type == GREATER_THAN || token->type == GREATER_THAN_EQUAL)) countRelOperatoes++;

        // If the number of relational operators is more than one, then is it error
        if(!continueReduction && (notEndDollar || beginDollar) && (countRelOperatoes > 1)){
            destroy_all(&stack,tree);
            return RET_VAL_SYNTAX_ERR;
        }
        
        // Counting of breackets in an expression
        if (!continueReduction && (notEndDollar || beginDollar) && token->type == BRACKET_RIGHT_SIMPLE) countBrac++;
        if ((notEndDollar || beginDollar) && token->type == BRACKET_LEFT_SIMPLE) countBrac--;

        // If there are more right brackets than left brackets, then is it error
        if (!continueReduction && (notEndDollar || beginDollar) && countBrac < 0){
            destroy_all(&stack,tree);
            return RET_VAL_SYNTAX_ERR;
        }

        // Simulation of a dollar in the stack
        if (beginDollar){
            precedence = getPrecedence(DOLLAR, precedenceIndex(token->type));
            beginDollar = false;
            notEndDollar= true;
        }
        else if(topTerminal == NULL && !beginDollar) precedence = getPrecedence(DOLLAR, precedenceIndex(token->type));
        else if (notEndDollar) precedence = getPrecedence(precedenceIndex(topTerminal->token->type), precedenceIndex(token->type));
        else precedence = getPrecedence(precedenceIndex(topTerminal->token->type), DOLLAR);
        
        if (precedence == LSS_COMP){
            continueReduction = false;
            continue;
        }
    
        

        // Switch for precedence analysis
        switch (precedence){

            // The closest terminal to top of stack has the same precedence as the input symbol(=)
            case EQ_COMP:
                if(stack_push(&stack, token, TERMINAL)){
                    destroy_all(&stack,tree);
                    return RET_VAL_INTERNAL_ERR;
                }
                break;

            // The closest terminal to top of stack has lower precedence than the input symbol(<) <=> shift
            case LSS_COMP:
                if(stack_insert_less(&stack, topTerminal)){
                    destroy_all(&stack,tree);
                    return RET_VAL_INTERNAL_ERR;
                }
                break;
            
            // The closest terminal to top of stack has higher precedence than the input symbol(>) <=> reduce
            case GR_COMP:

                // Number of reduce rul
                if (!canReduce(&stack)){
                    destroy_all(&stack,tree);
                    return RET_VAL_SYNTAX_ERR;
                }
                

                
                if(!reduce(&stack, tree, canReduce(&stack))){
                    destroy_all(&stack,tree);
                    return RET_VAL_INTERNAL_ERR;
                }
                

                if (stack_push(&stack, NULL, REDUCE)){
                    destroy_all(&stack,tree);
                    return RET_VAL_INTERNAL_ERR;
                }

                continueReduction = true;
                
                break;
            
            // Error
            case ERR:
                destroy_all(&stack,tree);
                return RET_VAL_SYNTAX_ERR;
                break;

        }
    
    }

    // Dispose stack (for peace of my soul)
    stack_dispose(&stack);

    // Retrun end of expression to buffer
    move_back(buffer);
    return RET_VAL_OK;
}

