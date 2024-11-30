#!/bin/bash

# Function to display help message
show_help() {
    echo "Usage: $0 <test_number|range>"
    echo
    echo "Arguments:"
    echo "  <test_number|range>  A single test number (e.g., 46) or a range (e.g., 46-66)."
    echo
    echo "Description:"
    echo "  This script creates test folders with 'input.ifj' and 'output.txt' files."
    echo "  The destination path will be requested interactively."
    echo
    echo "Options:"
    echo "  --help               Show this help message."
    exit 0
}

# Function to confirm the destination path
confirm_path() {
    echo "The tests will be created in: $DEST_PATH"
    read -p "Confirm? (y/n/m): " CONFIRM
    case "$CONFIRM" in
        y|Y)
            return 0
            ;;
        m|M)
            read -p "Enter new path: " DEST_PATH
            # Check if the path is valid
            if [ ! -d "$DEST_PATH" ]; then
                echo "Invalid path. Please enter a valid directory."
                confirm_path
            else
                echo "New path set: $DEST_PATH"
                confirm_path
            fi
            ;;
        *)
            echo "Operation cancelled."
            exit 1
            ;;
    esac
}

# Validate the range input
validate_range() {
    if [[ ! "$1" =~ ^[0-9]+(-[0-9]+)?$ ]]; then
        echo "Invalid input. Provide a single number or a range (e.g., 46 or 46-66)."
        exit 1
    fi
}

# Parse the range or number
parse_range() {
    if [[ "$1" == *-* ]]; then
        START="${1%-*}"
        END="${1#*-}"
        if [ "$START" -gt "$END" ]; then
            echo "Invalid range: Start number cannot be greater than end number."
            exit 1
        fi
        RANGE_TYPE="range"
    else
        START="$1"
        END="$1"
        RANGE_TYPE="single"
    fi
}

# Function to create tests
create_tests() {
    for ((i = START; i <= END; i++)); do
        DIR_NAME="$DEST_PATH/test$i"
        mkdir -p "$DIR_NAME"
        touch "$DIR_NAME/input.ifj" "$DIR_NAME/output.txt"
        echo "Created: $DIR_NAME"
    done
}

# Main script starts here
if [[ "$1" == "--help" ]]; then
    show_help
fi

if [ -z "$1" ]; then
    echo "Error: You must specify a test number or range. Use --help for usage information."
    exit 1
fi

# Validate and parse the input range
validate_range "$1"
parse_range "$1"

# Ask for destination path
read -p "Enter the destination path for the tests (default: current directory): " DEST_PATH
DEST_PATH="${DEST_PATH:-$(pwd)}"

# Check if the path is valid
if [ ! -d "$DEST_PATH" ]; then
    echo "Invalid path: $DEST_PATH"
    exit 1
fi

# Confirm the path
confirm_path

# Print confirmation message
if [ "$RANGE_TYPE" == "range" ]; then
    echo "Test${START}-${END} will be created. Press anything to cancel."
else
    echo "Test${START} will be created. Press anything to cancel."
fi
read -t 3 -n 1 CANCEL
if [ $? -eq 0 ]; then
    echo "Operation cancelled."
    exit 1
fi

# Create tests
create_tests

echo "All tests created successfully."
