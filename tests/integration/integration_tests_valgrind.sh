#!/bin/bash
# /** AUTHOR
#  * 
#  * @author <247581> Martin Mendl
#  * @file integration_tests.sh
#  * @date 25.11. 2024
#  * @brief This script runs integration tests for the given translator binary.
#  * @note this script is run by the run_test.sh
#  */

# Define color constants
COLOR_WARN="\033[1;33m"   # Yellow
COLOR_PASS="\033[1;32m"   # Green
COLOR_FAIL="\033[1;31m"   # Red
COLOR_RESET="\033[0m"     # Reset color

# Initialize test instance
function initTestInstance() {
    local testName=$1
    echo -e "\n\n${COLOR_WARN}=========== $testName ===========${COLOR_RESET}\n"
}

# Test case function
function testCase() {
    local testNum=$1
    local testName=$2
    local testResult=$3
    local passMsg=$4
    local failMsg=$5
    echo -e "TEST $testNum: $testName"
    if [ $testResult -eq 1 ]; then
        echo -e "  ${COLOR_PASS}PASS${COLOR_RESET}: $passMsg\n"
    else
        echo -e "  ${COLOR_FAIL}FAIL${COLOR_RESET}: $failMsg\n"
    fi
}

# Finish test instance
function finishTestInstance() {
    local totalTests=$1
    local passedTests=$2
    local failedTests=$((totalTests - passedTests))
    echo -e "\n${COLOR_WARN}=========== SUMMARY ===========${COLOR_RESET}"
    echo -e "${COLOR_PASS}PASSED${COLOR_RESET}: $passedTests/$totalTests"
    echo -e "${COLOR_FAIL}FAILED${COLOR_RESET}: $failedTests/$totalTests"
    if [ $failedTests -eq 0 ]; then
        echo -e "${COLOR_PASS}** ALL TESTS PASSED **${COLOR_RESET}"
        echo "Note: This is the part where we do a happy dance!"
    else
        echo -e "${COLOR_FAIL}SOME TESTS FAILED${COLOR_RESET}"
        echo "What are you doing here ?? !! Get back to work ..."
    fi
}

# Main script

# Parameters
binary=$1
testcase=$2
print=$3
valgrind=$4

# Redirect output to /dev/null if 'print' is false
if [ "$print" == "false" ] || [ "$testcase" = "-1" ]; then
    redirect_output="> /dev/null 2>&1"
else
    redirect_output=""
fi

echo -e "Running tests using binary: ${COLOR_WARN}$binary${COLOR_RESET}"

# Initialize test instance
testName="Integration Tests"
initTestInstance "$testName"

all_tests=0
passed_tests=0

# Check if testcase is "-1" or a specific test case
if [ "$testcase" == "-1" ]; then
  # Run all tests in the folder, output redirected to /dev/null

    # Find all test files, sort them numerically by test number
    for file in $(find ./test_inputs/integration -type f | sort -V); do
        valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --error-exitcode=100 ./$binary < "$file"
        rc=$?

        all_tests=$((all_tests + 1))

        # Extract test_num and expected_rc from the file name
        filename=$(basename "$file")
        test_num=$(echo "$filename" | cut -d'_' -f2)
        expected_rc=$(echo "$filename" | cut -d'_' -f3 | cut -d'.' -f1)

        # if rc = 100 report as mem leak
        if [ "$rc" -eq "100" ]; then
            testName="Test $filename"
            passMsg="Expected $expected_rc and got $rc"
            failMsg="Memory leak detected"
            testCase "$all_tests" "$testName" 0 "$passMsg" "$failMsg"
        else
            testName="Test $filename"
            passMsg="Expected $expected_rc and got $rc"
            failMsg="Expected $expected_rc but got $rc"
            if [ "$rc" -eq "$expected_rc" ]; then
                testCase "$all_tests" "$testName" 1 "$passMsg" "$failMsg"
                passed_tests=$((passed_tests + 1))
            else
                testCase "$all_tests" "$testName" 0 "$passMsg" "$failMsg"
            fi
        fi
    done
else
    # Run a specific test case
    file=$(find ./test_inputs/integration -type f -name "test_${testcase}_*.zig")
    if [ -f "$file" ]; then

        valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --error-exitcode=100 ./$binary < "$file"
        rc=$?
        
        all_tests=1
        filename=$(basename "$file")
        
        # Correctly parse the expected return code
        expected_rc=$(echo "$filename" | awk -F'_' '{print $3}' | cut -d'.' -f1)
        
        # if rc = 100 report as mem leak
        if [ "$rc" -eq "100" ]; then
            testName="Test $filename"
            passMsg="Expected $expected_rc and got $rc"
            failMsg="Memory leak detected"
            testCase "$all_tests" "$testName" 0 "$passMsg" "$failMsg"
        else
            testName="Test $filename"
            passMsg="Expected $expected_rc and got $rc"
            failMsg="Expected $expected_rc but got $rc"
            if [ "$rc" -eq "$expected_rc" ]; then
                testCase "$all_tests" "$testName" 1 "$passMsg" "$failMsg"
                passed_tests=$((passed_tests + 1))
            else
                testCase "$all_tests" "$testName" 0 "$passMsg" "$failMsg"
            fi
        fi
    else
        echo -e "${COLOR_FAIL}Testcase $testcase not found.${COLOR_RESET}"
    fi
fi

finishTestInstance "$all_tests" "$passed_tests"
