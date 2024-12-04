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
 * GRAMMAR:
 * 
 * S -> E
 * S -> R
 * 
 * E -> E + E
 * E -> E - E
 * E -> E*E
 * E -> E/E
 * E ->(E)
 * E -> id
 * E -> int_literal
 * E -> float_literal
 * E -> string_literal
 * E -> null
 * 
 * R -> E < E
 * R -> E > E
 * R -> E <= E
 * R -> E >= E
 * R -> E == E
 * R -> E != E 
 * R -> (R)
 * 
 */



// Precednce table 
PRECEDENCE precedence_table[14][14] = {
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
OPERATOR_INDEX prec_index_table(TOKEN_TYPE type){

    switch (type){
        case IDENTIFIER:return ID;
        case INT:return ID;
        case FLOAT:return ID;
        case STRING:return ID;
        case NULL_TOKEN:return ID;
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
PRECEDENCE get_precedence(OPERATOR_INDEX row, OPERATOR_INDEX coll){ 
    if(row != -1 && coll != -1) return precedence_table[row][coll];
    else return ERR;
}


/**
 * @brief Function for set count of reduce items in stack
 * @param stack Pointer on stack
 * @return Count of variables in stack
*/
int count_reduce(T_STACK_PTR stack){
    // Count of reduce items
    int count_of_r = 0;
    if (stack->count_items == 1) return 1;
    if (stack->count_items == 0) return 0;
        
    // Get top of stack
    T_STACK_ITEM_PTR starting_item = stack_top(stack);

    // Counting until the we find SHIFT or stack doesn't terminals and shifts
    while (starting_item != NULL && starting_item->type != SHIFT){
        count_of_r++;
        starting_item = starting_item->prev;

    }
    return count_of_r;
    
}


/**
 * @brief Function for search reduce rule
 * @param stack Pointer on stack
 * @return Number of type reduce rule, if rule doesn't exist return 0
*/
int can_reduce(T_STACK_PTR stack){

    // Count of reduce items
    int count_of_r = count_reduce(stack);

    // Rule doesn't exist
    if(count_of_r != 1 && count_of_r != 3) return 0;

    // E -> id | int_value | float_value
    if (count_of_r == 1 && stack->top->type == TERMINAL) return 1;
    // E -> R | T
    if (count_of_r == 1 && (stack->top->type == NON_TERMINAL_R || stack->top->type == NON_TERMINAL_E )) return 2;
    
    T_STACK_ITEM_PTR left = stack_top(stack);
    T_STACK_ITEM_PTR operator = left->prev;
    T_STACK_ITEM_PTR right = operator->prev;

    // E -> E + E | E - E | E * E | E / E  IT IS EQUAL TO 3
    if(count_of_r == 3 && (left->type == NON_TERMINAL_E && right->type == NON_TERMINAL_E) && (operator->token->type == PLUS || operator->token->type == MINUS || operator->token->type == MULTIPLY || operator->token->type == DIVIDE)) return 3;
    // T -> E < E | E > E | E <= E | E >= E | E == E | E != E IT IS EQUAL TO 4
    if (count_of_r == 3 && (left->type == NON_TERMINAL_E && right->type == NON_TERMINAL_E) && (operator->token->type == LESS_THAN || operator->token->type == GREATER_THAN || operator->token->type == LESS_THAN_EQUAL || operator->token->type == GREATER_THAN_EQUAL || operator->token->type == EQUAL || operator->token->type == NOT_EQUAL)) return 4;
    // E -> (E) IT IS EQUAL TO 5
    if (count_of_r == 3 && left->node->token->type == BRACKET_RIGHT_SIMPLE && (operator->type == NON_TERMINAL_E || operator->type == NON_TERMINAL_R) && right->node->token->type == BRACKET_LEFT_SIMPLE) return 5;

    return 0;


}

/**
 * @brief Function for reduce
 * @param stack Pointer on stack
 * @param tree Pointer on tree
 * @param rule Number of reduce rule
 * @param make_tree_flag Flag for create tree
 * @return True if reduce was successful, false if error
 */
bool reduce(T_STACK_PTR stack, T_TREE_NODE_PTR *tree, int rule, bool make_tree_flag){

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
            T_TREE_NODE_PTR neterminal_node= (stack_top(stack))->node;

            // Pop terminal
            stack_pop(stack);
            // Pop shift
            stack_pop(stack);

            // Push non terminal E
            if(stack_push(stack, NULL, NON_TERMINAL_E)) return false;

            // Set node of non terminal E
            (stack->top)->node = neterminal_node;    
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
            root = tree_create_sub_tree(operator->node, right->node, left->node);
            // If is end of expression, theen start creating tree
            if(make_tree_flag) *tree = root;

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
            root = tree_create_sub_tree(operator->node, right->node, left->node);
            // If is end of expression, theen start creating tree
            if (make_tree_flag) *tree = root;

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
 * @param type_end Type of end of expression
 * @parammake_tree_flag Flag for create tree
 * @return 0 if analysis is successful, 2 if syntax error, 99 if internal error (malloc for example)
*/
RET_VAL precedence_syntax_main(T_TOKEN_BUFFER *buffer, T_TREE_NODE_PTR *tree, TYPE_END type_end){

    // Create and init stack
    T_STACK stack;
    stack_init(&stack);

    // Count of brackets
    int count_brac = 0;
    // Count of relational operators
    unsigned int count_rel_operators = 0;

    // Flag for begin of expression
    bool begin_dollar = true;
    // Flag for end of expression, and for create tree
    bool not_end_dollar = false;
    // Flag for continue reduction
    bool conntionue_reduce = false;    

    // Token for analysis
    T_TOKEN *token = NULL;

    // Precedence of operators
    PRECEDENCE precedence = ERR;

    
    // Precednce analysis
    while(!is_empty(&stack) || begin_dollar){

        // Get token from buffer, until end of expression
        if(!conntionue_reduce && (not_end_dollar || begin_dollar)) next_token(buffer, &token);

        // Get the toppest terminal on stack
        T_STACK_ITEM_PTR topTerminal = stack_top_terminal(&stack);

        // Search for end of expression
        if(!conntionue_reduce && (not_end_dollar || begin_dollar) && ((token->type == BRACKET_RIGHT_SIMPLE && count_brac == 0 && count_rel_operators <= 1 && type_end == IF_WHILE_END) || (token->type == SEMICOLON && count_brac == 0 && count_rel_operators <= 1 && type_end == ASS_END))) not_end_dollar = false;

        // Counting relational operators in an expression  
        if (!conntionue_reduce && (not_end_dollar || begin_dollar) && (token->type == EQUAL || token->type == NOT_EQUAL || token->type == LESS_THAN || token->type == LESS_THAN_EQUAL || token->type == GREATER_THAN || token->type == GREATER_THAN_EQUAL)) count_rel_operators++;

        // If the number of relational operators is more than one, then is it error
        if(!conntionue_reduce && (not_end_dollar || begin_dollar) && (count_rel_operators > 1)){
            stack_dispose(&stack);
            return RET_VAL_SYNTAX_ERR;
        }
        
        // Counting of breackets in an expression
        if (!conntionue_reduce && (not_end_dollar || begin_dollar) && token->type == BRACKET_LEFT_SIMPLE) count_brac++;
        if (!conntionue_reduce && (not_end_dollar || begin_dollar) && token->type == BRACKET_RIGHT_SIMPLE) count_brac--;

        // If there are more right brackets than left brackets, then is it error
        if (!conntionue_reduce && (not_end_dollar || begin_dollar) && count_brac < 0){
            stack_dispose(&stack);
            return RET_VAL_SYNTAX_ERR;
        }

        // Simulation of a dollar on the stack
        if (begin_dollar){
            precedence = get_precedence(DOLLAR, prec_index_table(token->type));
            begin_dollar = false;
            not_end_dollar= true;
        }
        else if (topTerminal == NULL && !begin_dollar && not_end_dollar) precedence = get_precedence(DOLLAR, prec_index_table(token->type));
        else if (not_end_dollar) precedence = get_precedence(prec_index_table(topTerminal->token->type), prec_index_table(token->type));
        else if (!not_end_dollar && topTerminal == NULL && !begin_dollar && stack.count_items == 1 && precedence == GR_COMP) precedence = GR_COMP;
        else precedence = get_precedence(prec_index_table(topTerminal->token->type), DOLLAR);
        
        // End of raduction, if the precedence is greater than the input symbol
        if (precedence == LSS_COMP && conntionue_reduce){
            //Turn off the flag for continue reduction
            conntionue_reduce = false;

            // If is not end of expression, then push new terminal on stack after reduction
            if (!not_end_dollar) continue;
            
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
                conntionue_reduce = false;

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
                if (!can_reduce(&stack)){
                    stack_dispose(&stack);
                    return RET_VAL_SYNTAX_ERR;
                }
                

                
                if(!reduce(&stack, tree , can_reduce(&stack), !not_end_dollar)){
                    stack_dispose(&stack);
                    return RET_VAL_INTERNAL_ERR;
                }
                
                conntionue_reduce = true;
                
                break;
            
            // Error
            case ERR:
                stack_dispose(&stack);
                return RET_VAL_SYNTAX_ERR;
                break;

        }
    
    }
    
    
    // Dispose stack
    stack_dispose(&stack);

    // Retrun end of expression to buffer
    move_back(buffer);
    
    if ((*tree) == NULL) return RET_VAL_SYNTAX_ERR;
    // Return success
    else return RET_VAL_OK;
}

