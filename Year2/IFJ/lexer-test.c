/**
 * @file lexer-test.c
 * @authors Peter Rucek ...
 * @date 15 Oct 2020
 * @brief Lexer tests
 */

#include "lexer.h"
#include "error.h"

 char *arr[] = {
    "ID",
    "INT",
    "STRING",
    "FLOAT64",
    "KEYWORD",

    "PARENTHESIS_LEFT",   // (
    "PARENTHESIS_RIGHT",  // )
    "BRACKET_LEFT",       // {
    "BRACKET_RIGHT",      // }

    "SUB",
    "DIV",
    "ADD",
    "MUL",

    "COMMA",      // ,
    "DEF_OF_VAR", // :=
    "VAR_ASSIGN", // =
    "SEMICLN",    // ;

    "GT",        // >
    "LT",        // <
    "NE",        // !=
    "LE",        // <=
    "GE",        // >=
    "EQ",        // ==

    "EOL",
    "EoF",
 };
char *k_arr[] =
 {
    "K_INT",
    "K_STRING",
    "K_FLOAT64",
    "K_IF",
    "K_ELSE",
    "K_FOR",
    "K_FUNC",
    "K_PACKAGE",
    "K_RETURN",
    "K_INPUTS",
    "K_INPUTF",
    "K_INPUTI",
    "K_PRINT",
    "K_INT2FLOAT",
    "K_FLOAT2INT",
    "K_LEN",
    "K_SUBSTR",
    "K_ORD",
    "K_CHR",

};
void print(Token t)
{
    printf("Token type: %s\n",arr[t.type]);
    if (t.type == INT  || t.type == ERROR)
    {
       printf("Token data: %d\n",t.data.i);
    }
    else if (t.type == FLOAT64)
    {
       printf("Token data: %f\n",t.data.d);
    }
    else if (t.type == KEYWORD)
    {
       printf("Token data: %s\n",k_arr[t.data.k]);
    }
    else
    {
        printf("Token data: %s\n",t.data.s);
    }
}

int main ()
{
    Token t;
    t.type = -1; //porovnavanie s neinicializovanou premennou 
    while(t.type != EoF)
    {
        t = get_next_token(stdin);
        print(t);
    }

    return error_value;
}