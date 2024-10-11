#!/usr/bin/env python3
import subprocess
import json
import sys
import os
from difflib import unified_diff

# Path to the scanner executable
SCANNER_EXECUTABLE = '../../bin/scannerdebug'  # Update this path if necessary

# Path to the tests directory
TESTS_DIRECTORY = './'  # Update this path if necessary

def run_test(test_dir):
    test_name = os.path.basename(test_dir)
    input_file = os.path.join(test_dir, 'input.zig')
    expected_output_file = os.path.join(test_dir, 'expected_output.json')

    print(f'Running {test_name}...')

    # Read the input code
    try:
        with open(input_file, 'r') as f:
            input_code = f.read()
    except FileNotFoundError:
        print(f'Error: Input file not found: {input_file}')
        return False

    # Read the expected output
    try:
        with open(expected_output_file, 'r') as f:
            expected_output = json.load(f)
    except FileNotFoundError:
        print(f'Error: Expected output file not found: {expected_output_file}')
        return False
    except json.JSONDecodeError as e:
        print(f'Error: Failed to parse expected output JSON: {e}')
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
        print(f'Error: Scanner executable not found at {SCANNER_EXECUTABLE}')
        sys.exit(1)
    except Exception as e:
        print(f'Error: Could not start scanner executable: {e}')
        sys.exit(1)

    # Provide input to the scanner
    stdout_data, stderr_data = process.communicate(input=input_code)

    # Check for errors
    if process.returncode != 0:
        print(f'Error: Scanner exited with code {process.returncode}')
        print(f'Stderr:\n{stderr_data}')
        return False

    # Parse the JSON output
    try:
        output_tokens = json.loads(stdout_data)
    except json.JSONDecodeError as e:
        print(f'Error: Failed to parse JSON output: {e}')
        print(f'Scanner Output:\n{stdout_data}')
        return False

    # Compare the output with the expected output
    if output_tokens != expected_output:
        print(f'Test Failed: Output does not match expected output.')
        # Show the differences
        expected_str = json.dumps(expected_output, indent=2)
        output_str = json.dumps(output_tokens, indent=2)
        diff = unified_diff(
            expected_str.splitlines(keepends=True),
            output_str.splitlines(keepends=True),
            fromfile='expected',
            tofile='actual'
        )
        print('Differences:')
        print(''.join(diff))
        return False

    print('Test Passed.')
    return True

def main():
    all_passed = True

    # Get all test directories
    test_dirs = [os.path.join(TESTS_DIRECTORY, d) for d in os.listdir(TESTS_DIRECTORY)
                 if os.path.isdir(os.path.join(TESTS_DIRECTORY, d))]

    if not test_dirs:
        print(f'No tests found in directory {TESTS_DIRECTORY}')
        sys.exit(1)

    for test_dir in sorted(test_dirs):
        result = run_test(test_dir)
        print()
        if not result:
            all_passed = False

    if all_passed:
        print('All tests passed.')
    else:
        print('Some tests failed.')
        sys.exit(1)

if __name__ == '__main__':
    main()
