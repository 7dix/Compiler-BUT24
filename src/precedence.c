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
    /* ID */   { ERR,      GR_COMP,  GR_COMP,  GR_COMP,  GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   ERR,       GR_COMP,  GR_COMP },
    /* +  */   { LSS_COMP, GR_COMP,  GR_COMP,  LSS_COMP, LSS_COMP,  GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
    /* -  */   { LSS_COMP, GR_COMP,  GR_COMP,  LSS_COMP, LSS_COMP,  GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
    /* *  */   { LSS_COMP, GR_COMP,  GR_COMP,  GR_COMP,  GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
    /* /  */   { LSS_COMP, GR_COMP,  GR_COMP,  GR_COMP,  GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   GR_COMP,  GR_COMP,   GR_COMP,   LSS_COMP,  GR_COMP,  GR_COMP },
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
        case INT:return ID;
        case FLOAT:return ID;
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
 * @brief Function for set count of reduce items in stack
 * @param stack Pointer on stack
 * @return Count of variables in stack
*/
int reduceVaribles(T_STACK_PTR stack){
    // Count of reduce items
    int countOfVarRed = 0;
    if (stack->countItems == 1) return 1;
    if (stack->countItems == 0) return 0;
        
    // Get top of stack
    T_STACK_ITEM_PTR startingItem = stack_top(stack);

    // Counting until the we find SHIFT or stack doesn't terminals and shifts
    while (startingItem != NULL && startingItem->type != SHIFT){
        countOfVarRed++;
        startingItem = startingItem->prev;

    }
    return countOfVarRed;
    
}


/**
 * @brief Function for search reduce rule
 * @param stack Pointer on stack
 * @return Number of type reduce rule, if rule doesn't exist return 0
*/
int canReduce(T_STACK_PTR stack){

    // Count of reduce items
    int countOfVarRed = reduceVaribles(stack);

    // Rule doesn't exist
    if(countOfVarRed != 1 && countOfVarRed != 3) return 0;

    // E -> id | int_value | float_value
    if (countOfVarRed == 1 && stack->top->type == TERMINAL) return 1;
    // E -> R | T
    if (countOfVarRed == 1 && (stack->top->type == NON_TERMINAL_R || stack->top->type == NON_TERMINAL_E )) return 2;
    
    T_STACK_ITEM_PTR left = stack_top(stack);
    T_STACK_ITEM_PTR operator = left->prev;
    T_STACK_ITEM_PTR right = operator->prev;

    // E -> E + E | E - E | E * E | E / E  IT IS EQUAL TO 3
    if(countOfVarRed == 3 && (left->type == NON_TERMINAL_E && right->type == NON_TERMINAL_E) && (operator->token->type == PLUS || operator->token->type == MINUS || operator->token->type == MULTIPLY || operator->token->type == DIVIDE)) return 3;
    // T -> E < E | E > E | E <= E | E >= E | E == E | E != E IT IS EQUAL TO 4
    if (countOfVarRed == 3 && (left->type == NON_TERMINAL_E && right->type == NON_TERMINAL_E) && (operator->token->type == LESS_THAN || operator->token->type == GREATER_THAN || operator->token->type == LESS_THAN_EQUAL || operator->token->type == GREATER_THAN_EQUAL || operator->token->type == EQUAL || operator->token->type == NOT_EQUAL)) return 4;
    // E -> (E) IT IS EQUAL TO 5
    if (countOfVarRed == 3 && left->node->token->type == BRACKET_RIGHT_SIMPLE && (operator->type == NON_TERMINAL_E || operator->type == NON_TERMINAL_R) && right->node->token->type == BRACKET_LEFT_SIMPLE) return 5;

    return 0;


}

/**
 * @brief Function for reduce
 * @param stack Pointer on stack
 * @param tree Pointer on tree
 * @param rule Number of reduce rule
 * @param makeTree Flag for create tree
 * @return True if reduce was successful, false if error
 */
bool reduce(T_STACK_PTR stack, T_TREE_NODE_PTR *tree, int rule, bool makeTree){

    // Init variables
    T_STACK_ITEM_PTR operator = NULL;
    T_STACK_ITEM_PTR right = NULL;
    T_STACK_ITEM_PTR top = stack_top(stack);
    T_STACK_ITEM_PTR left = NULL;
    T_TREE_NODE_PTR root = NULL;

    switch (rule){
        // E -> id | int_value | float_value
        case 1:
        {
            // Get node of tree of reduce item
            T_TREE_NODE_PTR neterminalNode = (stack_top(stack))->node;

            // Pop terminal
            stack_pop(stack);
            // Pop shift
            stack_pop(stack);

            // Push non terminal E
            if(stack_push(stack, NULL, NON_TERMINAL_E)) return false;

            // Set node of non terminal E
            (stack->top)->node = neterminalNode;    
            return true;
        }

        // E -> R | T
        case 2:
        {
            // Get node of tree of reduce item
            T_TREE_NODE_PTR root = (stack_top(stack))->node;

            // Pop reduce item
            stack_pop(stack);
            // Pop shift item
            stack_pop(stack);

            // This is final state so node of reduce item is root of tree
            *tree = root;
            return true;
        }

        // E -> E + E | E - E | E * E | E / E
        case 3:
        {                
            // Get right neterminal
            right = top;
            // Get operator
            operator = top->prev;
            // Get left neterminal
            left = operator->prev;


            // Create subtree, of reduce item
            root = createSubTree(operator->node, right->node, left->node);
            // If is end of expression, theen start creating tree
            if(makeTree) *tree = root;

            // Pop right neterminal
            stack_pop(stack);
            // Pop operator
            stack_pop(stack);
            // Pop left neterminal
            stack_pop(stack);
            // Pop shift
            stack_pop(stack);

            // Push non terminal E
            if(stack_push(stack, NULL, NON_TERMINAL_E)) return false;

            // Set new node of non terminal E
            stack->top->node = root;
            return true;
        }

        // T -> E < E | E > E | E <= E | E >= E | E == E | E != E
        case 4:
        {   
            // Get right neterminal
            right = top;
            // Get operator
            operator = right->prev;
            // Get left neterminal
            left = operator->prev;

            // Create subtree, of reduce item
            root = createSubTree(operator->node, right->node, left->node);
            // If is end of expression, theen start creating tree
            if (makeTree) *tree = root;

            // Pop right neeterminal
            stack_pop(stack);
            // Pop operator
            stack_pop(stack);
            // Pop left neterminal
            stack_pop(stack);
            // Pop shift
            stack_pop(stack);
            
            // Push non terminal R
            if(stack_push(stack, NULL, NON_TERMINAL_R)) return false;

            // Set new node of non terminal R
            stack->top->node = root;
            return true;
        }

        // E -> (E) 
        case 5:
        {
            // Get neterminal
            T_STACK_ITEM_PTR neterminal = (stack_top(stack))->prev;
            STACK_ITEM_TYPE type = neterminal->type;
            root = neterminal->node;

            // Free node of right bracket
            free((stack_top(stack))->node);
            // Pop RB
            stack_pop(stack);
            // Pop Neterminal
            stack_pop(stack);
            // Free node of left bracket
            free((stack_top(stack))->node);
            // Pop LB
            stack_pop(stack);
            // Pop shift
            stack_pop(stack);

            

            // Push non terminal E
            
            if(stack_push(stack, NULL, type)) return false;
            // Set new node, what is root of tree
            stack->top->node = root;
            return true;
        }
    }

    return false;
}


/**
 * @brief Main function for precedence syntax analysis
 * @param buffer Pointer on buffer of tokens
 * @param tree Pointer on tree
 * @param typeEnd Type of end of expression
 * @param makeTree Flag for create tree
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
    // Flag for end of expression, and for create tree
    bool notEndDollar = false;
    // Flag for continue reduction
    bool continueReduction = false;    

    // Token for analysis
    T_TOKEN *token = NULL;

    // Precedence of operators
    PRECEDENCE precedence = ERR;

    
    // Precednce analysis
    while(!is_empty(&stack) || beginDollar){

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
            stack_dispose(&stack);
            return RET_VAL_SYNTAX_ERR;
        }
        
        // Counting of breackets in an expression
        if (!continueReduction && (notEndDollar || beginDollar) && token->type == BRACKET_LEFT_SIMPLE) countBrac++;
        if (!continueReduction && (notEndDollar || beginDollar) && token->type == BRACKET_RIGHT_SIMPLE) countBrac--;

        // If there are more right brackets than left brackets, then is it error
        if (!continueReduction && (notEndDollar || beginDollar) && countBrac < 0){
            stack_dispose(&stack);
            return RET_VAL_SYNTAX_ERR;
        }

        // Simulation of a dollar in the stack
        if (beginDollar){
            precedence = getPrecedence(DOLLAR, precedenceIndex(token->type));
            beginDollar = false;
            notEndDollar= true;
        }
        else if (topTerminal == NULL && !beginDollar && notEndDollar) precedence = getPrecedence(DOLLAR, precedenceIndex(token->type));
        else if (notEndDollar) precedence = getPrecedence(precedenceIndex(topTerminal->token->type), precedenceIndex(token->type));
        else if (!notEndDollar && topTerminal == NULL && !beginDollar && stack.countItems == 1 && precedence == GR_COMP) precedence = GR_COMP;
        else precedence = getPrecedence(precedenceIndex(topTerminal->token->type), DOLLAR);
        
        // End of raduction, if the precedence is greater than the input symbol
        if (precedence == LSS_COMP && continueReduction){
            //Turn off the flag for continue reduction
            continueReduction = false;

            // If is not end of expression, then push new terminal on stack after reduction
            if (!notEndDollar) continue;
            
            // Shift
            if (stack_insert_less(&stack, topTerminal)){
                stack_dispose(&stack);
                return RET_VAL_INTERNAL_ERR;
            }
            
            // Push new terminal on stack
            if(stack_push(&stack, token, TERMINAL)){
                stack_dispose(&stack);
                return RET_VAL_INTERNAL_ERR;
            }

            continue;
        }
    
        

        // Switch for precedence analysis
        switch (precedence){

            // The closest terminal to top of stack has the same precedence as the input symbol(=)
            case EQ_COMP:

                if(stack_push(&stack, token, TERMINAL)){
                    stack_dispose(&stack);
                    return RET_VAL_INTERNAL_ERR;
                }
                continueReduction = false;

                break;

            // The closest terminal to top of stack has lower precedence than the input symbol(<) <=> shift
            case LSS_COMP:
                if(stack_insert_less(&stack, topTerminal)){
                    stack_dispose(&stack);
                    return RET_VAL_INTERNAL_ERR;
                }
                if(stack_push(&stack, token, TERMINAL)){
                    stack_dispose(&stack);
                    return RET_VAL_INTERNAL_ERR;
                }
                break;
            
            // The closest terminal to top of stack has higher precedence than the input symbol(>) <=> reduce
            case GR_COMP:

                // Number of reduce rul
                if (!canReduce(&stack)){
                    stack_dispose(&stack);
                    return RET_VAL_SYNTAX_ERR;
                }
                

                
                if(!reduce(&stack, tree , canReduce(&stack), !notEndDollar)){
                    stack_dispose(&stack);
                    return RET_VAL_INTERNAL_ERR;
                }
                
                continueReduction = true;
                
                break;
            
            // Error
            case ERR:
                stack_dispose(&stack);
                return RET_VAL_SYNTAX_ERR;
                break;

        }
    
    }

    // Dispose stack (for peace of my soul)
    stack_dispose(&stack);

    // Retrun end of expression to buffer
    move_back(buffer);

    // Return success
    return RET_VAL_OK;
}

