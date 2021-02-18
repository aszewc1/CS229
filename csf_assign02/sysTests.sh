# aszewc1
# vwen2

#! /usr/bin/env bash

# System tests for postfix calculator (can test both C and asm versions)

# Check usage
if [ $# -ne 1 ]; then
    echo "Usage: ./sysTests.sh <exe name>"
    echo "  <exeName> should be './cPostfixCalc' or './asmPostfixCalc'"
    exit 1
fi

# Get the executable to test
exe="$1"

# Variables to keep track of test passed/attempted
numPassed=0
numAttempted=0

# Function testing that a postfix expression yields the expected result
expect() {
    local expected="$1"
    local expr="$2"

    if ./runTest.rb "$exe" "$expected" "$expr"; then
        numPassed=$(expr $numPassed + 1)
    fi
    numAttempted=$(expr $numAttempted + 1)
}

# Function testing that an invalid postfix expression yields an error
expect_error() {
    local expr="$1"

    if ./runTest.rb "$exe" "ERROR" "$expr"; then
        numPassed=$(expr $numPassed + 1)
    fi
    numAttempted=$(expr $numAttempted + 1)
}

#######################################################################
# Tests go here
#######################################################################

expect 5 '2 3 +'
expect 42 '6 7 *'
expect 42 '6 6 6 6 6 6 6 + + + + + +'
expect_error '2 2'
expect_error '1 *'

# Given on assignment page
expect 2 '1 1 +'
expect 5 '7 2 -'
expect 27 '3 4 5 + *'
expect 5 '17 3 /'
expect 0 '3 10 /'
expect -12 '2 3 4 5 +-*'
expect_error '10 2 - *'
expect_error '2 3 + 4'

# More tests
expect 20 '1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 + + + + + + + + + + + + + + + + + + +'
expect_error '1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 + + + + + + + + + + + + + + + + + + + +'
expect_error '2 3 44 5 6 9 + + + + + 1'
expect_error '2 3 44 5 6 9 + + + + 1 2 -'
expect 100594 '2 20 * 2 / 3 / 4 + 3 20 * * 6 + 15 - 100003 40 54 / + +'
expect_error '1 - - 4 5'
expect_error ''
expect_error '-'
expect 3 '3'
expect -100 '0 5 105 15 0   75 0 1 2 3 *
- -/  -+  + -+'
expect 10 '6 9 34 2 /+-20+69*420 42 /+'

#######################################################################
# End of tests
#######################################################################

# Summarize results
echo "$numPassed/$numAttempted tests passed"
if [ $numPassed -eq $numAttempted ]; then
    exit 0
else
    exit 1
fi

