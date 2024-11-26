#!/usr/bin/env python3
import subprocess
import json
import sys
import os
from difflib import unified_diff
from colorama import Fore, Style, init
from rich.console import Console
from rich.table import Table
from rich.box import ROUNDED  # Ensure ROUNDED is imported
from rich import print

# Initialize colorama for cross-platform color support
init(autoreset=True)

# Initialize rich console
console = Console()

# Path to the scanner executable
SCANNER_EXECUTABLE = '../../bin/scannerdebug'

# Path to the tests directory
TOKEN_CHECK_TESTS_DIRECTORY = './token_check' 
RET_CODE_TESTS_DIRECTORY = './ret_code' 

def colorize_status(status):
    """Colorize the status text for display."""
    if status == "SUCCESSFUL":
        return "[bold green]" + status + "[/bold green]"
    elif status == "FAILED":
        return "[bold red]" + status + "[/bold red]"
    elif status == "SKIPPED":
        return "[bold yellow]" + status + "[/bold yellow]"
    return status

def print_colored(text, color):
    """Print text with a specified color using rich."""
    print(f"[{color}]{text}[/{color}]")

def run_token_check_test(test_dir):
    test_name = os.path.basename(test_dir)
    input_file = os.path.join(test_dir, 'input.zig')
    expected_output_file = os.path.join(test_dir, 'expected_output.json')

    print_colored(f'Running {test_name}...', 'cyan')

    # Read the input code
    try:
        with open(input_file, 'r') as f:
            input_code = f.read()
    except FileNotFoundError:
        print_colored(f'Error: Input file not found: {input_file}', 'red')
        return False

    # Read the expected output
    try:
        with open(expected_output_file, 'r') as f:
            expected_output = json.load(f)
    except FileNotFoundError:
        print_colored(f'Error: Expected output file not found: {expected_output_file}', 'red')
        return False
    except json.JSONDecodeError as e:
        print_colored(f'Error: Failed to parse expected output JSON: {e}', 'red')
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
        print_colored(f'Error: Scanner executable not found at {SCANNER_EXECUTABLE}', 'red')
        sys.exit(1)
    except Exception as e:
        print_colored(f'Error: Could not start scanner executable: {e}', 'red')
        sys.exit(1)

    # Provide input to the scanner
    stdout_data, stderr_data = process.communicate(input=input_code)

    # Check for errors
    if process.returncode != 0:
        print_colored(f'Error: Scanner exited with code {process.returncode}', 'red')
        print_colored(f'Stderr:\n{stderr_data}', 'yellow')
        return False

    # Parse the JSON output
    try:
        output_tokens = json.loads(stdout_data)
    except json.JSONDecodeError as e:
        print_colored(f'Error: Failed to parse JSON output: {e}', 'red')
        print_colored(f'Scanner Output:\n{stdout_data}', 'yellow')
        return False

    # Compare the output with the expected output
    if output_tokens != expected_output:
        print_colored(f'Test Failed: Output does not match expected output.', 'red')
        # Show the differences
        expected_str = json.dumps(expected_output, indent=2)
        output_str = json.dumps(output_tokens, indent=2)
        diff = unified_diff(
            expected_str.splitlines(keepends=True),
            output_str.splitlines(keepends=True),
            fromfile='expected',
            tofile='actual'
        )
        print_colored(f'Differences:', 'magenta')
        print_colored(f'{"".join(diff)}', 'yellow')
        return False

    print_colored(f'Test Passed.', 'green')
    return True

def run_ret_code_test(test_dir):
    test_name = os.path.basename(test_dir)
    input_file = os.path.join(test_dir, 'input.ifj')
    expected_ret_code_file = os.path.join(test_dir, 'output.txt')

    print_colored(f'Running return code test {test_name}...', 'cyan')

    # Read the input code
    try:
        with open(input_file, 'r') as f:
            input_code = f.read()
    except FileNotFoundError:
        print_colored(f'Error: Input file not found: {input_file}', 'red')
        return False

    # Read the expected return code
    try:
        with open(expected_ret_code_file, 'r') as f:
            expected_ret_code = int(f.read().strip())
    except FileNotFoundError:
        print_colored(f'Error: Expected return code file not found: {expected_ret_code_file}', 'red')
        return False
    except ValueError:
        print_colored(f'Error: Invalid return code in file: {expected_ret_code_file}', 'red')
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
        print_colored(f'Error: Scanner executable not found at {SCANNER_EXECUTABLE}', 'red')
        sys.exit(1)
    except Exception as e:
        print_colored(f'Error: Could not start scanner executable: {e}', 'red')
        sys.exit(1)

    # Provide input to the scanner
    process.communicate(input=input_code)

    # Check the return code
    if process.returncode != expected_ret_code:
        print_colored(f'Test Failed: Return code {process.returncode} does not match expected {expected_ret_code}', 'red')
        return False

    print_colored(f'Return code test passed.', 'green')
    return True

def main():
    all_passed = True
    test_results = []

    # Get all token check test directories
    token_check_test_dirs = [os.path.join(TOKEN_CHECK_TESTS_DIRECTORY, d) for d in os.listdir(TOKEN_CHECK_TESTS_DIRECTORY)
                             if os.path.isdir(os.path.join(TOKEN_CHECK_TESTS_DIRECTORY, d))]

    if not token_check_test_dirs:
        print_colored(f'No token check tests found in directory {TOKEN_CHECK_TESTS_DIRECTORY}', 'red')
        sys.exit(1)

    for test_dir in sorted(token_check_test_dirs):
        result = run_token_check_test(test_dir)
        test_results.append(('token_check/'+str(os.path.basename(test_dir)), "SUCCESSFUL" if result else "FAILED"))
        print()
        if not result:
            all_passed = False

    # Get all return code test directories
    ret_code_test_dirs = [os.path.join(RET_CODE_TESTS_DIRECTORY, d) for d in os.listdir(RET_CODE_TESTS_DIRECTORY)
                          if os.path.isdir(os.path.join(RET_CODE_TESTS_DIRECTORY, d))]

    if not ret_code_test_dirs:
        print_colored(f'No return code tests found in directory {RET_CODE_TESTS_DIRECTORY}', 'red')
        sys.exit(1)

    for test_dir in sorted(ret_code_test_dirs):
        result = run_ret_code_test(test_dir)
        test_results.append(('ret_code/'+str(os.path.basename(test_dir)), "SUCCESSFUL" if result else "FAILED"))
        print()
        if not result:
            all_passed = False

    # Print Test Results Table
    table = Table(title="Test Results Summary", box=ROUNDED, border_style="bold blue")
    table.add_column("Test No.", style="bold", width=25)
    table.add_column("Result", justify="center", style="bold", width=12)

    # Sort test results numerically before printing
    test_results.sort(key=lambda x: x[0])  # Sort results by the test name

    for test_number, result in test_results:
        result_color = "green" if result == "SUCCESSFUL" else "red"
        table.add_row(test_number, f"[{result_color}]{result}[/{result_color}]")

    console.print(table)

    # Quick overview: Count successful, failed, and skipped tests
    success_count = sum(1 for _, result in test_results if result == "SUCCESSFUL")
    failed_count = sum(1 for _, result in test_results if result == "FAILED")

    # Print count of results in a separate table
    count_table = Table(title="Test Counts Overview", box=ROUNDED, border_style="bold blue")
    count_table.add_column("Status", style="bold", width=12)
    count_table.add_column("Count", justify="center", style="bold", width=12)

    # Add count data to the table with color-coded status
    count_table.add_row(
        "[bold green]SUCCESSFUL[/bold green]", f"[bold green]{success_count}[/bold green]"
    )
    count_table.add_row(
        "[bold red]FAILED[/bold red]", f"[bold red]{failed_count}[/bold red]"
    )

    # Print the count table
    console.print(count_table)

    if all_passed:
        print_colored('All tests passed.', 'green')
    else:
        print_colored('Some tests failed.', 'red')
        sys.exit(1)

if __name__ == '__main__':
    main()