import os
import re
import sys

def is_comment(line):
    """Returns True if the line is a comment."""
    return line.strip().startswith('//') or re.match(r'/\*.*\*/', line.strip())

def is_blank(line):
    """Returns True if the line is blank (empty)."""
    return len(line.strip()) == 0

def is_declaration(line):
    """Returns True if the line contains a declaration or a function prototype."""
    # Basic regex to capture variable declarations and function prototypes (limited)
    return bool(re.match(r'^\s*(int|char|float|double|void|long|short|struct|enum|typedef).*;', line.strip())) or \
           bool(re.match(r'^\s*(int|char|float|double|void|long|short|struct|enum|typedef)\s+\w+\s*\(.*\)\s*;', line.strip()))

def count_lines_in_file(file_path):
    """Counts the lines of code, declarations, comments, and blank lines in a file."""
    total_lines = 0
    lines_decl = 0
    lines_code = 0
    lines_blank = 0
    lines_comments = 0
    
    with open(file_path, 'r') as file:
        for line in file:
            total_lines += 1
            line = line.strip()

            # Check for blank lines
            if is_blank(line):
                lines_blank += 1
            # Check for comment lines (single-line or block comments)
            elif is_comment(line):
                lines_comments += 1
                # If the line contains a comment and code/declaration, count both
                if is_declaration(line):
                    lines_decl += 1
                    lines_code += 1
            # Check for declaration or code lines
            elif is_declaration(line):
                lines_decl += 1
                lines_code += 1
            else:
                lines_code += 1  # Any line with code counts as a code line

    return total_lines, lines_decl, lines_code, lines_blank, lines_comments

def scan_directory_for_code_files(directory):
    """Scans the directory for .c and .h files and processes them."""
    total_lines_all_files = 0
    lines_decl_all_files = 0
    lines_code_all_files = 0
    lines_blank_all_files = 0
    lines_comments_all_files = 0

    # Traverse the directory for .c and .h files
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.c') or file.endswith('.h'):
                file_path = os.path.join(root, file)
                total, decl, code, blank, comments = count_lines_in_file(file_path)
                
                total_lines_all_files += total
                lines_decl_all_files += decl
                lines_code_all_files += code
                lines_blank_all_files += blank
                lines_comments_all_files += comments

    # Calculate percentages
    code_percentage = (lines_code_all_files / total_lines_all_files) * 100 if total_lines_all_files > 0 else 0
    comment_percentage = (lines_comments_all_files / total_lines_all_files) * 100 if total_lines_all_files > 0 else 0
    blank_percentage = (lines_blank_all_files / total_lines_all_files) * 100 if total_lines_all_files > 0 else 0

    # Print the results
    print(f"LINES TOTAL: {total_lines_all_files}")
    print(f"LINES DECL: {lines_decl_all_files}")
    print(f"LINES CODE: {lines_code_all_files}")
    print(f"LINES BLANK: {lines_blank_all_files}")
    print(f"LINES COMMENTS: {lines_comments_all_files}")
    print(f"CODE PERC: {code_percentage:.2f}%")
    print(f"COMMENT PERC: {comment_percentage:.2f}%")
    print(f"BLANK PERC: {blank_percentage:.2f}%")

# Run the script on a specified directory
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python script.py <directory_path>")
        sys.exit(1)
    
    directory = sys.argv[1]
    
    if not os.path.isdir(directory):
        print(f"The specified path '{directory}' is not a valid directory.")
        sys.exit(1)

    scan_directory_for_code_files(directory)
