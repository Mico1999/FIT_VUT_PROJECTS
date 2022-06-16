/**
 * @file error.h
 * @authors Marek Micek, Matej Jurik
 * @date 16 OCT 2020
 * @brief Interface for the error program
 */

#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

extern int error_value; //< Represents exit code of compiler
extern int lasterror_line; //< Represents last line on which error was thrown

enum eError {
    ERR_LEX = 1, //
    ERR_SYN,
    ERR_DEF,
    ERR_TYPE,
    ERR_COMP,
    ERR_PARAM,
    ERR_OTHR,
    ERR_ZERODIV = 9,
    ERR_INTRN = 99
};

// Error interface

void lexical_error(int line);
void syntax_error(int token_type, int line);
void no_definition_error(char *id, int line);
void re_definition_error(char *id, int line);
void re_definition_error2(char *id, int line);
void type_error(char *id, char *type, int line);
void compatibility_error(char *type, int line);
void param_error(char *id, int line);       // ERR_PARAM 6
void main_definition_error(int line);       // ERR_PARAM 6
void param_num_error(char *id, int line);   // ERR_PARAM 6
void param_type_error(char *id, int line);  // ERR_PARAM 6
void return_num_error(char *id, int line);  // ERR_PARAM 6
void return_type_error(char *id, int line); // ERR_PARAM 6
void return_unpack_error(char *id, int line);       // ERR_PARAM 6
void void_func_retvals_error(char *id, int line);   // ERR_PARAM 6
void no_return_error(char *id, int line);           // ERR_PARAM 6
void other_error(int line);
void div_zero_error(int line);
void intern_error();


#endif