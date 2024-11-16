import os
import subprocess
import sys

# Mapping error codes to their textual representation
ERROR_CODES = {
    0: "OK",
    1: "LEXICAL ERROR",
    2: "SYNTAX ERROR",
    99: "INTERNAL ERROR"
}

# The path to the parser executable
PARSER_EXEC = os.path.join(os.path.dirname(__file__), '../../bin/ifj24debug')

# ANSI escape codes for color formatting
COLOR_SUCCESS = '\033[92m'  # Green
COLOR_FAILED = '\033[91m'   # Red
COLOR_SKIPPED = '\033[93m'  # Yellow
COLOR_HEADER = '\033[94m'   # Blue
COLOR_RESET = '\033[0m'     # Reset to default

def print_colored(text, color):
    """Print text with a specified color."""
    print(f"{color}{text}{COLOR_RESET}")

# Enhanced run_test function with color output
def run_test(test_folder, test_number):
    # Construct the paths for input and expected output files
    input_file = os.path.join(test_folder, f'test{test_number}', 'input.ifj')
    expected_output_file = os.path.join(test_folder, f'test{test_number}', 'output.txt')

    if not os.path.exists(input_file) or not os.path.exists(expected_output_file):
        print_colored(f"Skipping test {test_number}: Missing input or expected output file.", COLOR_SKIPPED)
        return test_number, "SKIPPED"

    # Run the parser with the input file
    result = subprocess.run(
        [PARSER_EXEC],  # Run the parser executable
        input=open(input_file, 'r').read(),  # Providing input from the file
        text=True,
        capture_output=True
    )

    # Read expected output
    with open(expected_output_file, 'r') as f:
        expected_output = f.read().strip()

    # Get the actual return code
    actual_output = str(result.returncode)  # Get the return code as a string

    # Compare expected and actual outputs
    if actual_output == expected_output:
        print_colored(f"TEST {test_number} SUCCESSFUL", COLOR_SUCCESS)
        return test_number, "SUCCESSFUL"
    else:
        # Translate the return code to text
        actual_text = ERROR_CODES.get(int(actual_output), "UNKNOWN ERROR")
        expected_text = ERROR_CODES.get(int(expected_output), "UNKNOWN ERROR")
        print_colored(f"TEST {test_number} FAILED", COLOR_FAILED)
        print(f"  Expected: {expected_text} (Code: {expected_output})")
        print(f"  Got: {actual_text} (Code: {actual_output})")
        return test_number, "FAILED"

# Enhanced run_tests function with color output
def run_tests(test_type):
    # Define base path for the tests based on the argument
    base_path = ''
    if test_type == '-noexpr':
        base_path = os.path.join(os.path.dirname(__file__), 'no_expr')
    elif test_type == '-normal':
        base_path = os.path.join(os.path.dirname(__file__), 'normal')
    else:
        print("Invalid argument. Use -noexpr or -normal.")
        return

    test_results = []
    # Loop through the folders (test1, test2, ..., testn)
    for test_folder in os.listdir(base_path):
        test_folder_path = os.path.join(base_path, test_folder)
        if os.path.isdir(test_folder_path):
            test_number = test_folder.replace('test', '')  # Extract the number part (e.g., "test1" -> "1")
            result = run_test(base_path, test_number)
            test_results.append(result)

    # Print Summary
    print_colored("\nTest Summary:", COLOR_HEADER)
    print(f"{'Test No.':<10}{'Result':<12}")
    print("-" * 24)
    for test_number, result in test_results:
        result_color = COLOR_SKIPPED if result == "SKIPPED" else COLOR_SUCCESS if result == "SUCCESSFUL" else COLOR_FAILED
        print_colored(f"{test_number:<10}{result:<12}", result_color)
    print("-" * 24)

    # Quick overview: Count successful, failed, and skipped tests
    success_count = sum(1 for _, result in test_results if result == "SUCCESSFUL")
    failed_count = sum(1 for _, result in test_results if result == "FAILED")
    skipped_count = sum(1 for _, result in test_results if result == "SKIPPED")

    # Print color-coded quick overview with color-coded labels
    print(f"{COLOR_SUCCESS}SUCCESSFUL:{COLOR_RESET} {COLOR_SUCCESS}{success_count}{COLOR_RESET}")
    print(f"{COLOR_FAILED}FAILED:{COLOR_RESET} {COLOR_FAILED}{failed_count}{COLOR_RESET}")
    print(f"{COLOR_SKIPPED}SKIPPED:{COLOR_RESET} {COLOR_SKIPPED}{skipped_count}{COLOR_RESET}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py [-noexpr | -normal]")
        sys.exit(1)

    test_type = sys.argv[1]
    run_tests(test_type)
