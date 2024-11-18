#!/usr/bin/env python3
import subprocess
import json
import sys
import os
from difflib import unified_diff
from colorama import Fore, Style, init

# Initialize colorama for cross-platform color support
init(autoreset=True)

# Path to the scanner executable
SCANNER_EXECUTABLE = '../../bin/scannerdebug'  # Update this path if necessary

# Path to the tests directory
TESTS_DIRECTORY = './'  # Update this path if necessary

def run_test(test_dir):
    test_name = os.path.basename(test_dir)
    input_file = os.path.join(test_dir, 'input.zig')
    expected_output_file = os.path.join(test_dir, 'expected_output.json')

    print(f'{Style.BRIGHT}{Fore.CYAN}Running {test_name}...{Style.RESET_ALL}')

    # Read the input code
    try:
        with open(input_file, 'r') as f:
            input_code = f.read()
    except FileNotFoundError:
        print(f'{Fore.RED}Error: Input file not found: {input_file}{Style.RESET_ALL}')
        return False

    # Read the expected output
    try:
        with open(expected_output_file, 'r') as f:
            expected_output = json.load(f)
    except FileNotFoundError:
        print(f'{Fore.RED}Error: Expected output file not found: {expected_output_file}{Style.RESET_ALL}')
        return False
    except json.JSONDecodeError as e:
        print(f'{Fore.RED}Error: Failed to parse expected output JSON: {e}{Style.RESET_ALL}')
        return False

    # Run the scanner executable
    try:
        process = subprocess.Popen(
            [SCANNER_EXECUTABLE],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True
        )
    except FileNotFoundError:
        print(f'{Fore.RED}Error: Scanner executable not found at {SCANNER_EXECUTABLE}{Style.RESET_ALL}')
        sys.exit(1)
    except Exception as e:
        print(f'{Fore.RED}Error: Could not start scanner executable: {e}{Style.RESET_ALL}')
        sys.exit(1)

    # Provide input to the scanner
    stdout_data, stderr_data = process.communicate(input=input_code)

    # Check for errors
    if process.returncode != 0:
        print(f'{Fore.RED}Error: Scanner exited with code {process.returncode}{Style.RESET_ALL}')
        print(f'{Fore.YELLOW}Stderr:\n{stderr_data}{Style.RESET_ALL}')
        return False

    # Parse the JSON output
    try:
        output_tokens = json.loads(stdout_data)
    except json.JSONDecodeError as e:
        print(f'{Fore.RED}Error: Failed to parse JSON output: {e}{Style.RESET_ALL}')
        print(f'{Fore.YELLOW}Scanner Output:\n{stdout_data}{Style.RESET_ALL}')
        return False

    # Compare the output with the expected output
    if output_tokens != expected_output:
        print(f'{Fore.RED}Test Failed: Output does not match expected output.{Style.RESET_ALL}')
        # Show the differences
        expected_str = json.dumps(expected_output, indent=2)
        output_str = json.dumps(output_tokens, indent=2)
        diff = unified_diff(
            expected_str.splitlines(keepends=True),
            output_str.splitlines(keepends=True),
            fromfile='expected',
            tofile='actual'
        )
        print(f'{Fore.MAGENTA}Differences:{Style.RESET_ALL}')
        print(f'{Fore.YELLOW}{"".join(diff)}{Style.RESET_ALL}')
        return False

    print(f'{Fore.GREEN}Test Passed.{Style.RESET_ALL}')
    return True

def main():
    all_passed = True

    # Get all test directories
    test_dirs = [os.path.join(TESTS_DIRECTORY, d) for d in os.listdir(TESTS_DIRECTORY)
                 if os.path.isdir(os.path.join(TESTS_DIRECTORY, d))]

    if not test_dirs:
        print(f'{Fore.RED}No tests found in directory {TESTS_DIRECTORY}{Style.RESET_ALL}')
        sys.exit(1)

    for test_dir in sorted(test_dirs):
        result = run_test(test_dir)
        print()
        if not result:
            all_passed = False

    if all_passed:
        print(f'{Fore.GREEN}All tests passed.{Style.RESET_ALL}')
    else:
        print(f'{Fore.RED}Some tests failed.{Style.RESET_ALL}')
        sys.exit(1)

if __name__ == '__main__':
    main()