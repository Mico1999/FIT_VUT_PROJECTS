/**
 * @file codegenerator-test.c
 * @authors Peter Rucek ...
 * @date 13 Nov 2020
 * @brief Tests for code generation
 */

#include "codegenerator.h"

int main()
{
    gen_header();
    gen_func_header("main");
    gen_main_end();
    flush();

    return 0;
}