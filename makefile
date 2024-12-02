# Makefile for compiling C files in the src directory

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Debug flags
DEBUG_FLAGS = -g -O0

# Source files
SRC = src/main.c src/scanner.c src/token_buffer.c src/parser.c src/first_phase.c src/semantic.c src/semantic_list.c src/precedence.c src/precedence_stack.c src/precedence_tree.c src/symtable.c src/generate.c src/gen_handler.c
SRC_SCANNER_TEST = tests/src/main_test_scanner.c src/scanner.c
SRC_TOKEN_BUFFER_TEST = tests/src/main_test_token_buffer.c src/token_buffer.c src/scanner.c
SRC_PRECEDENCE_TEST = tests/src/main_test_precedence.c src/scanner.c src/token_buffer.c src/precedence.c src/precedence_stack.c src/precedence_tree.c 
SRC_SYMTABLE_TEST = tests/src/main_test_symtable.c src/symtable.c
SRC_FIRST_PHASE_TEST = tests/src/main_test_first_phase.c src/scanner.c src/token_buffer.c src/first_phase.c src/symtable.c
SRC_IN_FROM_FILE = tests/src/main_test.c src/scanner.c src/token_buffer.c src/parser.c src/first_phase.c src/semantic.c src/semantic_list.c src/precedence.c src/precedence_stack.c src/precedence_tree.c src/symtable.c src/generate.c src/gen_handler.c

# Output executables
OUTPUT = bin/ifj24
DEBUG_IN_FROM_FILE = bin/ifj24file
DEBUG_OUTPUT = bin/ifj24debug
DEBUG_SCANNER_OUTPUT = bin/scannerdebug
DEBUG_TOKEN_BUFFER_OUTPUT = bin/tokenbufferdebug
DEBUG_PRECEDENCE_OUTPUT = bin/precedencedebug
DEBUG_SYMTABLE_OUTPUT = bin/symtabledebug
DEBUG_FIRST_PHASE_OUTPUT = bin/firstphasedebug

LOGIN = x253206

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

debug_symtable: bin $(SRC_SYMTABLE_TEST)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_SYMTABLE_OUTPUT) $(SRC_SYMTABLE_TEST)

debug_first_phase: bin $(SRC_FIRST_PHASE_TEST)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_FIRST_PHASE_OUTPUT) $(SRC_FIRST_PHASE_TEST)
# Debug target for from_file
debug_from_file: bin $(SRC_IN_FROM_FILE)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -o $(DEBUG_IN_FROM_FILE) $(SRC_IN_FROM_FILE)


# Test targets
test_scanner: debug_scanner
	cd tests/scanner &&  python3 test_scanner.py

test_token_buffer: debug_token_buffer
	cd tests/scanner &&  python3 test_token_buffer.py

test_parser_retcode: debug
	cd tests/parser &&  python3 test_parser.py -retcode

test_parser_valgrind: debug
	cd tests/parser &&  python3 test_parser_valgrind.py -retcode

test_interpret: debug
	cd tests/parser &&  python3 test_interpret_interactive.py -retcode

test_precedence: debug_precedence 
	cd tests/precedence && python3 test_precedence.py

test_symtable: debug_symtable
	bin/symtabledebug

test_first_phase: debug_first_phase
	cd tests/first_phase && python3 test_first_phase.py -pubfn

test: test_scanner test_token_buffer test_precedence test_symtable test_parser_retcode																																																					 

test_fit: debug
	./tests/IFJ24-tests-master/test.sh ./tests/IFJ24-tests-master ./bin/ifj24debug ic24int																																																						

test_martin_valgrind: debug
	cd tests/integration && ./integration_tests_valgrind.sh "../../bin/ifj24debug" -1 true true

test_martin: debug
	cd tests/integration && ./integration_tests.sh "../../bin/ifj24debug" -1 true true

# Clean target to remove the executables
clean:
	rm -f $(OUTPUT) $(DEBUG_OUTPUT) $(DEBUG_SCANNER_OUTPUT) $(DEBUG_TOKEN_BUFFER_OUTPUT) $(DEBUG_FIRST_PHASE_OUTPUT) $(DEBUG_SYMTABLE_OUTPUT) $(DEBUG_PRECEDENCE_OUTPUT)
	rm -f $(LOGIN).zip
	rm -f *vgcore*
	rm -rf temp
	rm -rf IFJ24-tests-master/out
	rm -rf tests/IFJ24-tests-master/out
	rm -rf tests/parser/valgrind_output.txt

.PHONY: all debug clean bin test pack test_scanner test_token_buffer test_parser_retcode test_precedence test_symtable test_first_phase test debug_from_file debug_scanner debug_token_buffer debug_precedence debug_symtable debug_first_phase

pack:
	mkdir temp
	cp -r src/* temp/
	cp hand_in/Makefile temp/Makefile
	cp hand_in/dokumentace.pdf temp/dokumentace.pdf
	cp hand_in/rozdeleni temp/rozdeleni
	cd temp && zip $(LOGIN).zip *
	mv temp/$(LOGIN).zip .
	rm -r temp

check-pack:
	hand_in/is_it_ok.sh $(LOGIN).zip temp
	rm -r temp