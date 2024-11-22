
SRC = main.c scanner.c token_buffer.c parser.c first_phase.c precedence.c precedence_stack.c precedence_tree.c symtable.c semantic.c generate.c gen_handler.c
OUT = ifj24
CC = gcc
CFLAGS = -Wall -Wextra -Werror

build: $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC)
