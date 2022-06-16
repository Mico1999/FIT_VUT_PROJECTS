/**
 * @file testlib.c
 * @authors Matej Jurik
 * @date 16 Nov 2020
 * @brief Global interface for compiler test suites
 */

#ifndef __TESTLIB_H_
#define __TESTLIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

// Standard test suite return values

#define TEST_SUCCESS 0
#define TEST_FAIL 255 // -1

// Standard test interface

void setUp();
void tearDown();

#endif // __TESTLIB_H_