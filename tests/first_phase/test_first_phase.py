import os
import subprocess
import sys
from rich import print
from rich.console import Console
from rich.table import Table
from rich.box import ROUNDED  # Ensure ROUNDED is imported
from colorama import init

# Initialize colorama for colored console output
init(autoreset=True)

# Mapping error codes to their textual representation
ERROR_CODES = {
    0: "OK",
    1: "LEXICAL ERROR",
    2: "SYNTAX ERROR",
    99: "INTERNAL ERROR"
}

# The path to the parser executable
FIRSTPHASE_EXEC = os.path.join(os.path.dirname(__file__), '../../bin/firstphasedebug')

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

def print_colored(text, color):
    """Print text with a specified color using rich."""
    print(f"[{color}]{text}[/{color}]")

def run_test(test_folder, test_number):
    # Construct the paths for input and expected output files
    input_file = os.path.join(test_folder, f'test{test_number}', 'input.ifj')
    expected_output_file = os.path.join(test_folder, f'test{test_number}', 'output.txt')

    if not os.path.exists(input_file) or not os.path.exists(expected_output_file):
        print_colored(f"Skipping test {test_number}: Missing input or expected output file.", "yellow")
        return test_number, "SKIPPED"

    # Run the parser with the input file
    result = subprocess.run(
        [FIRSTPHASE_EXEC],  # Run the parser executable
        input=open(input_file, 'r').read(),  # Providing input from the file
        text=True,
        capture_output=True
    )

    # Read expected output
    with open(expected_output_file, 'r') as f:
        expected_output = f.read().strip()

    # Get actual output and convert both to sets of lines
    actual_output = result.stdout.strip()
    expected_lines = set(expected_output.splitlines())
    actual_lines = set(actual_output.splitlines())
    
    # Compare sets of lines
    if expected_lines == actual_lines:
        print_colored(f"TEST {test_number} SUCCESSFUL", "green")
        return test_number, "SUCCESSFUL"
    else:
        print_colored(f"TEST {test_number} FAILED", "red")
        print(f"  Expected: \n{expected_output}")
        print(f"  Got: \n{actual_output}")
        return test_number, "FAILED"

def run_tests(test_type):
    # Define base path for the tests based on the argument
    base_path = ''
    if test_type == '-pubfn':
        base_path = os.path.join(os.path.dirname(__file__), 'pub_fn')
        print("[bold cyan]Using configuration: No Expression Tests (pubfn)[/bold cyan]")
    else:
        print("Invalid argument. Use -noexpr or -normal.")
        return

    # Print basic info
    print("[bold cyan]Running tests for first_phase...[/bold cyan]\n")

    test_results = []
    # Loop through the folders (test1, test2, ..., testn) sorted numerically
    test_folders = [folder for folder in os.listdir(base_path) if os.path.isdir(os.path.join(base_path, folder))]
    test_folders.sort(key=lambda x: int(x.replace('test', '')))  # Sort by the numeric part of the folder name

    for test_folder in test_folders:
        test_number = test_folder.replace('test', '')  # Extract the number part (e.g., "test1" -> "1")
        result = run_test(base_path, test_number)
        test_results.append(result)

    # Print Test Results Table
    table = Table(title="Test Results Summary", box=ROUNDED, border_style="bold blue")  # Use ROUNDED here
    table.add_column("Test No.", style="bold", width=10)
    table.add_column("Result", justify="center", style="bold", width=12)

    # Sort test results numerically before printing
    test_results.sort(key=lambda x: int(x[0]))  # Sort results by the test number

    for test_number, result in test_results:
        result_color = "yellow" if result == "SKIPPED" else "green" if result == "SUCCESSFUL" else "red"
        table.add_row(test_number, f"[{result_color}]{result}[/{result_color}]")

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

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py [-pubfn]")
        sys.exit(1)

    test_type = sys.argv[1]
    run_tests(test_type)
