#!/bin/bash

# Test case definitions
declare -a test_inputs=(
    "var_name"  # Test 1: Valid Identifiers
    "123variable"  # Test 2: Invalid Identifier
    "123\n3.14\n2E10"  # Test 3: Valid Numeric Literals
    "0123\n1..23"  # Test 4: Invalid Numeric Literals
    "\"hello\"\n\"Ahoj\\n\\\"Sve'te \\\\\"\""  # Test 5: Valid String Literals
    "\"unterminated\n\"hello\\z\""  # Test 6: Invalid String Literals
    "\"Line1\\\\nLine2\"\n\"\\x41\\x42\\x43\""  # Test 7: Escape Sequences in Strings
    "\"hello\\y\""  # Test 8: Invalid Escape Sequences
    "const\nfn\nif\nreturn"  # Test 9: Keywords
    "+\n-\n*\n/\n{\n}\n="  # Test 10: Operators and Symbols
    "// This is a comment"  # Test 11: Comments
    "\"Hello \\\\\\nWorld\""  # Test 12: Multiline Strings
    "\"Hello \\nWorld\""  # Test 13: Invalid Multiline String
    "var x: i32 = 42;"  # Test 14: Combination of Valid Tokens
    "var x: intt = 42;"  # Test 15: Invalid Token Combination
    "ifj.write(\"Hello\");"  # Test 16: Special Keywords
    "var x: ?i32 = null;"  # Test 17: Null Values
    "   var  x :   i32=42;   "  # Test 18: Whitespace and Formatting
    "x"  # Test 19: Single Character Identifiers
    "@"  # Test 20: Invalid Single Character
    "if variable fn"  # Test 21: Combination of Keywords and Identifiers
    "// This is a comment // Nested"  # Test 22: Nested Comments
    "// Unclosed comment"  # Test 23: Invalid Comment Syntax
    "var x: ?f64 = null;"  # Test 24: Valid Nullable Types
    "var x: ?unknown = null;"  # Test 25: Invalid Nullable Types
    "var x: i32;\nvar y: f64;"  # Test 26: Multiple Lines
    "var x: i3&2;"  # Test 27: Invalid Character in Type Declaration
    "ifj.write(\"Hello, World!\");"  # Test 28: Valid Function Call
    "unknown.write(\"Test\");"  # Test 29: Invalid Function Call
    "var valid: i32; @invalidToken;"  # Test 30: Mixed Valid and Invalid Tokens
    "1.23E10"  # Test 31: Valid Floating Point Exponent
    "1.23E+"  # Test 32: Invalid Floating Point Exponent
    "var str: []u8 = ifj.concat(\"Hello\", \"World\");"  # Test 33: Valid String Concatenation
    "var str: []u8 = ifj.concat(123, \"World\");"  # Test 34: Invalid String Concatenation
    "var x: i32 = 10;;;;"  # Test 35: Extra Semicolons
    "var x: i32 = 10"  # Test 36: Missing Semicolon
    "\"Text with escape: \\n\\x41\""  # Test 37: Escape Sequence Edge Case
    "\"Text with invalid escape: \\q\""  # Test 38: Invalid Escape Sequence
    "Fn main() void {}"  # Test 39: Case Sensitivity in Keywords
    "var xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx: i32 = 42;"  # Test 40: Too Long Identifiers
)

declare -a expected_outputs=(
    "0"  # Test 1
    "1"  # Test 2
    "0"  # Test 3
    "1"  # Test 4
    "0"  # Test 5
    "1"  # Test 6
    "0"  # Test 7
    "1"  # Test 8
    "0"  # Test 9
    "0"  # Test 10
    "0"  # Test 11
    "0"  # Test 12
    "1"  # Test 13
    "0"  # Test 14
    "1"  # Test 15
    "0"  # Test 16
    "0"  # Test 17
    "0"  # Test 18
    "0"  # Test 19
    "1"  # Test 20
    "0"  # Test 21
    "0"  # Test 22
    "0"  # Test 23
    "0"  # Test 24
    "1"  # Test 25
    "0"  # Test 26
    "1"  # Test 27
    "0"  # Test 28
    "1"  # Test 29
    "1"  # Test 30
    "0"  # Test 31
    "1"  # Test 32
    "0"  # Test 33
    "1"  # Test 34
    "1"  # Test 35
    "1"  # Test 36
    "0"  # Test 37
    "1"  # Test 38
    "1"  # Test 39
    "1"  # Test 40
)

# Create test directories and files
for i in "${!test_inputs[@]}"; do
    test_num=$((i + 1))
    test_dir="test${test_num}"
    
    # Create directory
    mkdir -p "${test_dir}"
    
    # Create input file
    echo -e "${test_inputs[i]}" > "${test_dir}/input.ifj"
    
    # Create output file
    echo "${expected_outputs[i]}" > "${test_dir}/output.txt"
done

echo "Test cases created successfully!"