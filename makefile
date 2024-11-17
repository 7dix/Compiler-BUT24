# Makefile for compiling C files in the src directory

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Debug flags
DEBUG_FLAGS = -g -O0

# Source files
SRC = src/main.c src/scanner.c src/token_buffer.c src/parser.c src/first_phase.c
SRC_SCANNER_TEST = src/main_test_scanner.c src/scanner.c
SRC_TOKEN_BUFFER_TEST = src/main_test_token_buffer.c src/token_buffer.c src/scanner.c
SRC_PRECEDENCE_TEST = src/main_test_precedence.c src/scanner.c src/token_buffer.c src/precedence.c src/precedence_stack.c src/precedence_tree.c 

# Output executables
OUTPUT = bin/ifj24
DEBUG_OUTPUT = bin/ifj24debug
DEBUG_SCANNER_OUTPUT = bin/scannerdebug
DEBUG_TOKEN_BUFFER_OUTPUT = bin/tokenbufferdebug
DEBUG_PRECEDENCE_OUTPUT = bin/precedencedebug

# Default target
all: $(OUTPUT)

# Ensure bin directory exists
bin:
	mkdir -p bin

# Link object files to create the executable
$(OUTPUT): bin $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

# Debug target
debug: bin $(SRC)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_OUTPUT) $(SRC)

# Debug target for scanner test
debug_scanner: bin $(SRC_SCANNER_TEST)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_SCANNER_OUTPUT) $(SRC_SCANNER_TEST)

debug_token_buffer: bin $(SRC_TOKEN_BUFFER_TEST)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_TOKEN_BUFFER_OUTPUT) $(SRC_TOKEN_BUFFER_TEST)
debug_precedence: bin $(SRC_PRECEDENCE_TEST)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_PRECEDENCE_OUTPUT) $(SRC_PRECEDENCE_TEST)
test_scanner: debug_scanner
	cd tests/scanner &&  python3 test_scanner.py

test_token_buffer: debug_token_buffer
	cd tests/token_buffer &&  python3 test_token_buffer.py

test_parser_noexpr: debug
	cd tests/parser &&  python3 test_parser.py -noexpr

test_parser_normal: debug
	cd tests/parser &&  python3 test_parser.py -normal
test_precedence: debug_precedence 
	cd src
test: test_scanner test_token_buffer

# Clean target to remove the executables
clean:
	rm -f $(OUTPUT) $(DEBUG_OUTPUT) $(DEBUG_SCANNER_OUTPUT) $(DEBUG_TOKEN_BUFFER_OUTPUT)

.PHONY: all debug clean bin test