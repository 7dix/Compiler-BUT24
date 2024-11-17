import os
import subprocess
import difflib
import re
from rich import print
from rich.console import Console
from rich.table import Table
from rich.box import ROUNDED
from colorama import init, Fore, Style

# Initialize colorama for colored console output
init(autoreset=True)

# Define paths relative to the script's location
ROOT_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))  # root/ directory
EXECUTABLE_PATH = os.path.join(ROOT_DIR, 'bin', 'precedencedebug')  # root/bin/precedencedebug
TESTS_DIR = os.path.dirname(__file__)  # root/tests/precedence/ (script.py location)

# Initialize a rich Console object for printing
console = Console()

# Helper functions for colored output using rich
def print_success(msg):
    print(f"[bold green]{msg}[/bold green]")

def print_failure(msg):
    print(f"[bold red]{msg}[/bold red]")

def print_skipped(msg):
    print(f"[bold yellow]{msg}[/bold yellow]")

def print_error(msg):
    print(f"[bold red]ERROR: {msg}[/bold red]")

def compare_outputs(actual, expected):
    """Compare actual and expected outputs and return a diff if they don't match."""
    diff = list(difflib.unified_diff(expected.splitlines(), actual.splitlines()))
    if diff:
        return "\n".join(diff)
    return None

def colorize_status(status):
    """Colorize the status text for display in the table."""
    if status == "SUCCESSFUL":
        return "[bold green]" + status + "[/bold green]"
    elif status == "FAILED":
        return "[bold red]" + status + "[/bold red]"
    elif status == "SKIPPED":
        return "[bold yellow]" + status + "[/bold yellow]"
    return status

def run_test(test_id, input_path, output_path):
    """Run a single test by comparing actual vs expected output."""
    # Check if input and output files exist
    if not os.path.exists(input_path):
        print_skipped(f"Test {test_id} SKIPPED: Input file not found at {input_path}")
        return "SKIPPED", None, None

    if not os.path.exists(output_path):
        print_skipped(f"Test {test_id} SKIPPED: Output file not found at {output_path}")
        return "SKIPPED", None, None

    # Run the executable with input file as stdin
    with open(input_path, 'r') as input_file:
        try:
            result = subprocess.run([EXECUTABLE_PATH], stdin=input_file, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            actual_output = result.stdout
            return_code = result.returncode
        except Exception as e:
            print_error(f"Test {test_id} failed with exception: {e}")
            return "FAILED", None, str(e)

    # Read the expected output
    with open(output_path, 'r') as expected_file:
        expected_output = expected_file.read()

    # If the executable failed (non-zero exit code), treat as failed
    if return_code != 0:
        print_failure(f"Test {test_id} FAILED: Executable returned a non-zero exit code.")
        return "FAILED", actual_output, expected_output

    # Compare the outputs
    diff = compare_outputs(actual_output, expected_output)
    if diff:
        print_failure(f"Test {test_id} FAILED: Output differs from expected.\n" + diff)
        return "FAILED", actual_output, expected_output
    else:
        print_success(f"Test {test_id} SUCCESSFUL")
        return "SUCCESSFUL", actual_output, expected_output

def run_tests():
    """Run all tests and print a summary."""
    test_results = []
    failed_tests = 0
    skipped_tests = 0
    successful_tests = 0

    # Define a regex pattern for valid test directories (e.g., test1, test2, ...)
    test_dir_pattern = re.compile(r"^test(\d+)$")

    # Print basic info
    print("[bold cyan]Running tests for expressions...[/bold cyan]\n")
    
    # Get all the test directories and sort them numerically
    test_dirs = [d for d in os.listdir(TESTS_DIR) if test_dir_pattern.match(d)]
    test_dirs.sort(key=lambda x: int(test_dir_pattern.match(x).group(1)))  # Sort numerically

    # Iterate through the test directories (test1, test2, ...) and only consider valid ones
    for test_dir in test_dirs:
        test_path = os.path.join(TESTS_DIR, test_dir)
        input_file_path = os.path.join(test_path, 'input.expr')  # input file is input.expr
        output_file_path = os.path.join(test_path, 'output.txt')  # expected output file is output.txt

        if os.path.isdir(test_path):
            print(f"[bold cyan]Running Test {test_dir}...[/bold cyan]")  # Indicate running test
            status, actual_output, expected_output = run_test(test_dir, input_file_path, output_file_path)
            test_results.append([test_dir, colorize_status(status)])

            # Update counts based on the test result
            if status == "SUCCESSFUL":
                successful_tests += 1
            elif status == "FAILED":
                failed_tests += 1
            else:
                skipped_tests += 1

    # Print summary table of test results with rich styling and bold borders
    table = Table(title="Test Results Summary", box=ROUNDED, border_style="bold blue")
    table.add_column("Test ID", style="bold", width=12)
    table.add_column("Status", justify="center", style="bold", width=12)

    # Add rows to the table
    for test in test_results:
        table.add_row(test[0], test[1])

    console.print(table)

    # Print count of test results (SUCCESSFUL, FAILED, SKIPPED) with rich styling
    count_table = Table(title="Test Counts", box=ROUNDED, border_style="bold blue")
    count_table.add_column("Status", style="bold", width=12)
    count_table.add_column("Count", justify="center", style="bold", width=12)

    result_counts = [
        ["SUCCESSFUL", successful_tests],
        ["FAILED", failed_tests],
        ["SKIPPED", skipped_tests],
    ]

    # Colorize the count numbers for the summary table
    for count in result_counts:
        status = count[0]
        count_value = count[1]

        if status == "SUCCESSFUL":
            count_table.add_row(f"[bold green]{colorize_status(status)}[/bold green]", f"[bold green]{count_value}[/bold green]")
        elif status == "FAILED":
            count_table.add_row(f"[bold red]{colorize_status(status)}[/bold red]", f"[bold red]{count_value}[/bold red]")
        elif status == "SKIPPED":
            count_table.add_row(f"[bold yellow]{colorize_status(status)}[/bold yellow]", f"[bold yellow]{count_value}[/bold yellow]")

    console.print(count_table)

if __name__ == "__main__":
    run_tests()
