/**
 * @file dynamicstring.h
 * @authors Rebeka Cernianska
 * @date 16 Oct 2020
 * @brief Inteface for dynamicstring.c program
 */

#ifndef _DYNAMICSTRING_H_
#define _DYNAMICSTRING_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "error.h"

#define BLOCK 8

typedef struct dynamic_string
{
    int len; //< Buffer current size
    int alloc_len; //< Buffer (max) size
    char *buff;
} dynamic_string;

// Dynamicstring interface

void dynstr_init(dynamic_string *str);

int add_char(dynamic_string *str, char c);

int add_string(dynamic_string *str, char *str_to_copy);

int add_strings(dynamic_string *str, ...);

void dynstr_free(dynamic_string *str);

int dynstr_cmp(dynamic_string *dyn_str, const char *const_str);

void copy_token_string(char *token_string, dynamic_string *string);

#endif