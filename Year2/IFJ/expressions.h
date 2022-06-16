/**
 * @file expressions.h
 * @authors Peter Rucek
 * @date 16 Nov 2020
 * @brief Interface for expressions
 */

#ifndef _EXPRESSIONS_H_
#define _EXPRESSIONS_H_

#include "parser.h"
#include "stack.h"

//// Relocated typedef struct Terminal  to parser.h (cyclic dependency)

typedef enum
{
    SHIFT,
    REDUC,
    T_ERR,
    EQUAL
} TabItem;

#define TABLE_SIZE 14
#define MAX_STRING 1000

// Rules:
// E -> (E)     
// E -> id      |
// E -> int     |> i
// E -> string  |
// E -> float   |
// E -> E + E
// E -> E - E
// E -> E * E
// E -> E / E
// E -> E == E
// E -> E != E
// E -> E > E
// E -> E < E
// E -> E >= E
// E -> E <= E

// Table:
// ___||_(_||_)_||_+-_||_*/_||_r_||_i_||_$_||
//  ( || < || = || <  || <  || < || < ||   || 
//  ) ||   || > || >  || >  || > ||   || > ||
// +- || < || > || >  || <  || > || < || > ||
// */ || < || > || >  || >  || > || < || > ||
//  r || < || > || <  || <  ||   || < || > ||
//  i || FC|| > || >  || >  || > ||   || > || !!!
//  $ || < ||   || <  || <  || < || < || OK|| !!!


bool expr(Data_type *expr_type, bool *func_call, unsigned num_of_id, bool is_bool, bool can_be_empty);

bool reduce(Stack *s);

int find_terminal(Stack *s);

#endif