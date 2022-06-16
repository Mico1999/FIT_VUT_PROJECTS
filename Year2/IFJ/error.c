/**
 * @file error.c
 * @authors Marek Micek, Matej Jurik
 * @date 16 OCT 2020
 * @brief Error functions
 */

#include "error.h"

#define CAN_THROW_ERROR(curr_line) (lasterror_line < (curr_line))
#define THROW_ERROR(fmt, error_no, ...) \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    if (!error_value) error_value = (error_no)

char *array[] = {
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

int error_value = 0; //< in case of no error
int lasterror_line = 0;

/**
 * @brief Inform about lexical error occurrence
 * @param line Line of error causing lexeme
 * @post error_value = 1
 */
void lexical_error(int line)
{
    fprintf(stderr, "** Lexical error on line = '%d' ! **\n", line);
    if (error_value == 0)
    {
        error_value = ERR_LEX;
    }
}


void syntax_error(int token_type, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Syntax error: token type '%s', line %d! **\n", ERR_SYN, array[token_type], line);
        lasterror_line = line;
    }
}


void no_definition_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Identifier: '%s' on line %d was not defined yet ! **\n", ERR_DEF, id, line);
        lasterror_line = line;
    }
}


void re_definition_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Identifier: '%s' on line %d is already defined ! **\n", ERR_DEF, id, line);
        lasterror_line = line;
    }
}

void re_definition_error2(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Can't call function '%s' on line %d in this scope ! **\n", ERR_DEF, id, line);
        lasterror_line = line;
    }
}



void type_error(char *id, char *type, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Cannot assign '%s' to '%s' on line %d ! **\n", ERR_TYPE, type, id, line);
        lasterror_line = line;
    }
}


void compatibility_error(char *type, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Incompatible type assignment '%s' on line %d ! **\n", ERR_COMP, type, line);
        lasterror_line = line;
    }
}

void param_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Unmatched number or type of paramethers/return values in function: '%s' on line: %d ! **\n", ERR_PARAM, id, line);
        lasterror_line = line;
    }
}

void main_definition_error(int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: The function '%s' can not have any parameters or return values ! **\n", ERR_PARAM, "main");
        lasterror_line = line;
    }
}

void param_num_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Unmatched number of parameter values in function: '%s' on line: %d ! **\n", ERR_PARAM, id, line);
        lasterror_line = line;
    }
}


void param_type_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Unmatched type/s of parameter value/s in function: '%s' on line: %d ! **\n", ERR_PARAM, id, line);
        lasterror_line = line;
    }
}


void return_num_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Unmatched number of return values in function: '%s' on line: %d ! **\n", ERR_PARAM, id, line);
        lasterror_line = line;
    }
}


void return_type_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Unmatched type/s of return value/s in/when calling function: '%s' on line: %d ! **\n", ERR_PARAM, id, line);
        lasterror_line = line;
    }
}

void return_unpack_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Wrong number of IDs on left side for function: '%s' to unpack return values to. Line: %d ! **\n", ERR_PARAM, id, line);
        lasterror_line = line;
    }
}


// Handles primarily functions that have header retvals specified, yet do not return anything
// Throws error even when a return error was already thrown
void no_return_error(char *id, int line)
{
    if (CAN_THROW_ERROR(line))
    {
        THROW_ERROR("** Semantic error: Function: '%s' does not return what it should on line: %d ! **\n", ERR_PARAM, id, line);
        lasterror_line = line;
    }
}

void other_error(int line)
{
    fprintf(stderr, "** Another kind of semantic error on line: %d! **\n", line);
    if (error_value == 0)
    {
        error_value = ERR_OTHR;
    }
}


void div_zero_error(int line)
{
    fprintf(stderr, "** Semantic error: Division with zero on line: %d! **\n", line);
    if (error_value == 0)
    {
        error_value = ERR_ZERODIV;
    }
}


void intern_error()
{
    fprintf(stderr, "** Intern error of compiler ! **\n");
    error_value = ERR_INTRN;
}
