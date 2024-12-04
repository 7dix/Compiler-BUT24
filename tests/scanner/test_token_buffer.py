#!/usr/bin/env python3
import subprocess
import json
import sys
import os
from difflib import unified_diff
from rich import print
from rich.console import Console
from rich.table import Table
from rich.box import ROUNDED
from colorama import init

# Initialize colorama for colored console output
init(autoreset=True)

# Path to the scanner executable
SCANNER_EXECUTABLE = '../../bin/tokenbufferdebug'  # Update this path if necessary

# Path to the tests directory
TOKEN_CHECK_TESTS_DIRECTORY = './token_check'  # Update this path if necessary

# Initialize a rich Console object for printing
console = Console()

def colorize_status(status):
    """Colorize the status text for display."""
    if status == "SUCCESSFUL":
        return "[bold green]" + status + "[/bold green]"
    elif status == "FAILED":
        return "[bold red]" + status + "[/bold red]"
    elif status == "SKIPPED":
        return "[bold yellow]" + status + "[/bold yellow]"
    return status

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
        return test_name, "SKIPPED"

    # Read the expected output
    try:
        with open(expected_output_file, 'r') as f:
            expected_output = json.load(f)
    except FileNotFoundError:
        print(f'Error: Expected output file not found: {expected_output_file}')
        return test_name, "SKIPPED"
    except json.JSONDecodeError as e:
        print(f'Error: Failed to parse expected output JSON: {e}')
        return test_name, "SKIPPED"

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
        print(f'Error: Token Buffer executable not found at {SCANNER_EXECUTABLE}')
        sys.exit(1)
    except Exception as e:
        print(f'Error: Could not start Token Buffer executable: {e}')
        sys.exit(1)

    # Provide input to the scanner
    stdout_data, stderr_data = process.communicate(input=input_code)

    # Check for errors
    if process.returncode != 0:
        print(f'Error: Token Buffer exited with code {process.returncode}')
        print(f'Stderr:\n{stderr_data}')
        return test_name, "FAILED"

    # Parse the JSON output
    try:
        output_tokens = json.loads(stdout_data)
    except json.JSONDecodeError as e:
        print(f'Error: Failed to parse JSON output: {e}')
        print(f'Token Buffer Output:\n{stdout_data}')
        return test_name, "FAILED"

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
        return test_name, "FAILED"

    print(f'Test Passed: {test_name}')
    return test_name, "SUCCESSFUL"

def main():
    all_passed = True

    # Get all test directories
    test_dirs = [os.path.join(TOKEN_CHECK_TESTS_DIRECTORY, d) for d in os.listdir(TOKEN_CHECK_TESTS_DIRECTORY)
                 if os.path.isdir(os.path.join(TOKEN_CHECK_TESTS_DIRECTORY, d))]

    if not test_dirs:
        print(f'No tests found in directory {TOKEN_CHECK_TESTS_DIRECTORY}')
        sys.exit(1)

    # Track the results of each test
    test_results = []

    # Run tests and store results
    for test_dir in sorted(test_dirs):
        test_name, result = run_test(test_dir)
        test_results.append((test_name, result))
        print()
        if result != "SUCCESSFUL":
            all_passed = False

    # Print Test Results Summary Table
    table = Table(title="Test Results Summary", box=ROUNDED, border_style="bold blue")
    table.add_column("Test No.", style="bold", width=10)
    table.add_column("Result", justify="center", style="bold", width=12)

    # Sort test results numerically before printing
    test_results.sort(key=lambda x: int(x[0].replace('test', '')))  # Sort results by the test number

    for test_name, result in test_results:
        result_color = "yellow" if result == "SKIPPED" else "green" if result == "SUCCESSFUL" else "red"
        table.add_row(test_name, f"[{result_color}]{result}[/{result_color}]")

    console.print(table)

    # Quick overview: Count successful, failed, and skipped tests
    success_count = sum(1 for _, result in test_results if result == "SUCCESSFUL")
    failed_count = sum(1 for _, result in test_results if result == "FAILED")
    skipped_count = sum(1 for _, result in test_results if result == "SKIPPED")

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
    count_table.add_row(
        "[bold yellow]SKIPPED[/bold yellow]", f"[bold yellow]{skipped_count}[/bold yellow]"
    )

    # Print the count table
    console.print(count_table)

    # Final status
    if all_passed:
        print(f"\nAll tests passed.")
    else:
        print(f"\nSome tests failed.")
        sys.exit(1)

if __name__ == '__main__':
    main()
