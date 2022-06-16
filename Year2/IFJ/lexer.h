/**
 * @file lexer.h
 * @authors Peter Rucek, Rebeka Cernianska, Marek Micek, Matej Jurik
 * @date 15 Oct 2020
 * @brief Interface for the lexer
 */

#ifndef _LEXER_H_
#define _LEXER_H_

#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "dynamicstring.h"
#include "error.h"

extern const char *keywords[];

/**
 * @brief keywords[index] == Keyword state
 */
typedef enum
{
    K_ERROR = -1,
    K_INT,
    K_STRING,
    K_FLOAT64,
    K_IF,
    K_ELSE,
    K_FOR,
    K_FUNC,
    K_PACKAGE,
    K_RETURN

} Keyword;

typedef enum
{
    ERROR = -1,     // in case of lexical error
    ID,
    INT,
    STRING,
    FLOAT64,
    KEYWORD,

    PARENTHESIS_LEFT,   // (
    PARENTHESIS_RIGHT,  // )
    BRACKET_LEFT,       // {
    BRACKET_RIGHT,      // }

    SUB,
    DIV,
    ADD,
    MUL,

    COMMA,      // ,
    DEF_OF_VAR, // :=
    VAR_ASSIGN, // =
    SEMICLN,    // ;

    GT,        // >
    LT,        // <
    NE,        // !=
    LE,        // <=
    GE,        // >=
    EQ,        // ==

    EOL,
    EoF,       // ??

} Token_type;

typedef enum
{
    S_START,
    S_B_COMMENT,
    S_B_COMMENT_END,
    S_L_COMMENT,
    S_NUM,
    S_INT,
    S_STRING,
    S_ESC, // "\n", "\"", "\t", "\\", "\xhh"
    S_ESCHEX_1, // "\x--"
    S_ESCHEX_2, // "\xh-"
    S_DOUBLE,
    S_EXPO_1,
    S_EXPO_2,
    S_EXPO_3,
    S_ERROR,
    S_ID_OR_KEY,
    S_COM_OR_DIV,
    S_LT,
    S_GT,
    S_NE,
    S_ASGN_OR_EQ,
    S_DEF

} State;


typedef union
{
    Keyword k;
    char *s;
    int i;
    double d;

} Data;


typedef struct token
{
    Token_type type;
    Data data;

} Token;


// Lexer interface

Token get_next_token(FILE *f);
Keyword get_keywordID(char *str);

#endif