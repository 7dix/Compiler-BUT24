# Makefile for compiling C files in the src directory

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Debug flags
DEBUG_FLAGS = -g -O0

# Source files
SRC = $(wildcard src/*.c)

# Output executables
OUTPUT = bin/ifj24
DEBUG_OUTPUT = bin/ifj24debug

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

# Clean target to remove the executables
clean:
	rm -f $(OUTPUT) $(DEBUG_OUTPUT)

.PHONY: all debug clean bin