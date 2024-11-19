
#include "precedence.h"
#include "token_buffer.h"
#include "scanner.h"
#include "precedence_tree.h"
#include "precedence_tree.h"


int main(void)
{
    T_TREE treeStruct;
    tree_init(&treeStruct);

    T_TOKEN_BUFFER *buffer = init_token_buffer();
    if (buffer == NULL) {
        fprintf(stderr, "Error: Memory allocation failed in init_token_buffer\n");
        return 1;
    }



    int err_code = 0;
    T_TOKEN *currentToken;
    // Fill the buffer with tokens
    while (err_code == 0) {
        currentToken = (T_TOKEN *) malloc(sizeof(T_TOKEN));
        if (currentToken == NULL) {
            fprintf(stderr, "Error: Memory allocation failed in main\n");
            break;
        }
        err_code = get_token(currentToken);
        if (err_code != 0){
            fprintf(stderr, "Error: Lexical error at line %d\n", currentToken->line);
            break;
        }

        // Add the token to the buffer
        if (!add_token_as_last(buffer, currentToken)) {
            fprintf(stderr, "Error: Memory allocation failed in add_token_as_last\n");
            break;
        }

        if (currentToken->type == EOF_TOKEN){
            break;
        }
    }
    if (err_code != 0) {
        printf("\n]\n"); // End of JSON array
        fprintf(stderr, "..Compiling failed with exit code %d\n", err_code);
    }

    set_current_to_first(buffer);
    
    //printf("Precedence syntax analysis\n");
    RetVal ret = precedenceSyntaxMain(buffer, &treeStruct, ASS_END);
    
    if(ret == 0){
        T_TREE_NODE_PTR tree = getTree(&treeStruct); // Funkce getTree vrací přímo ukazatel na uzel
        printf("OK\n");
        postorder(tree); // Předání správného typu do postorder
        tree_dispose(&treeStruct);
    }else{
        printf("NOK\n");
    }


    
    free_token_buffer(&buffer);

    return 0;
}
