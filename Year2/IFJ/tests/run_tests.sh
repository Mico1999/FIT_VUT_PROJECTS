#!/usr/bin/env bash

################################################################
##  Compiles & runs every test suite (dependent on makefile)  ##
##               and displays results summary                 ##
##                                                            ##
##                    Author: Matej Jurik                     ##
################################################################

######################
##  Config & funcs  ##
######################

IFS=$'\n' # Internal Field Separator

TEST_SUCCESS=0    # errno cleared
TEST_FAIL=255     # errno custom (failed tests)
TEST_SEGFAULT=139 # errno segfault

GREEN="[1;32m"
RED="[1;31m"

GREP_MAX_COUNT=15

setUp() {
    # Change process working dir to ./tests 
    cd ./tests 

    make -s clean
    make -s
}

tearDown() {
    make -s clean
    rm -f _*.out

    # Exit out of ./tests
    cd ..

    # Reset IFS to spaces
    unset IFS 
}

# Accepts: $1 = color 
#          $2 = text to print
print() {
    printf "\033$1"
    printf $2
    printf "\033[0m"
}

# Accepts: $1 = color 
#          $2 = text to print
println() {
    printf "\033$1"
    echo $2
    printf "\033[0m"
}

# Accepts: $1 = array of faults
printFaults() {
    for fault in $@; do
        printf "  "
        echo $fault
    done
}

# Accepts: $1 = Num of fails occured 
print_num_fails() {
    if [[ $1 -ge $GREP_MAX_COUNT ]]; then
        println $RED " ($1 or more)"
    else 
        println $RED " ($1)"
    fi
}

###################
### Script Body ###
###################

setUp

# Debug option - compile test suites only
if [[ $1 == "--dbg" ]]; then
    cd ..
    unset IFS
    exit 0;
fi

# Test execution & parsing output
tests=(`find ./test_* -type f -print`)
echo "Running module tests: ${tests[@]}"
echo

for test in ${tests[@]}; do
    echo -n "Testing ${test:2}: "

    outfile="./${test:6}.out"

    # Execute test
    { $test 2>$outfile; } 2>>$outfile # stderr append any signals received by shell
    exitCode=$?

    #### Test suite exit codes branching
    ###      0: OK
    ###    255: TESTFAIL
    ###  Other: CRASH (special case: SEGFAULT)
    ####

    if [[ $exitCode -eq $TEST_SUCCESS ]]; then # Test ran successfully
        
        println $GREEN "OK!"
    
    elif [[ $exitCode -eq $TEST_FAIL ]]; then # Some tests failed

        fails=(`grep -m $GREP_MAX_COUNT TESTFAIL $outfile`)

        print $RED "FAILED!"
        print_num_fails ${#fails[@]}
        
        # Print current test's failures
        printFaults ${fails[@]}

    else # Execution error (Program crashed = badly written test suite or compiler)
        
        if [[ $exitCode -eq $TEST_SEGFAULT ]]; then
            println $RED "Segmentation Fault! (debug with 'make-test-debug')"
        else 
            # Split to array elements on linefeeds
            errors=(`grep -m $GREP_MAX_COUNT TESTERROR $outfile`)
            
            # A non-segfault error occured inside of test
            println $RED "ERROR! (debug with 'make-test-debug')"
            
            # Print current test's errors
            printFaults ${errors[@]}
        fi
    fi

done

tearDown
exit 0