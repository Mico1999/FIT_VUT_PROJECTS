/**
 * @file test_codegen.c
 * @authors Matej Jurik
 * @date 16 Nov 2020
 * @brief Codegenerator test suite
 */

#include "test_codegen.h"

// Standard macros & globals
#define setError() if(fail!=true) fail=true

#define testerror(error) perror("**TESTERROR " __FILE__ "** "error); exit(errno)
#define testfail(fmt, ...) \
    setError(); \
    fprintf(stderr, "**TESTFAIL " __FILE__"> %s** "fmt"\n", __func__, ##__VA_ARGS__)

bool fail = false;


int main(void)
{
    if (fail) return TEST_FAIL;
    return TEST_SUCCESS;
}