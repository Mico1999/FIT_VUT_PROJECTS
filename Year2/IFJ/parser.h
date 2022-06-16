/**
 * @file parser.h
 * @authors Peter Rucek, Matej Jurik, Rebeka Cernianska, Marek Micek
 * @date 25 Oct 2020
 * @brief Interface for the parser
 */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "error.h"
#include "symtable.h"
#include "codegenerator.h"
#include "stack.h"
#include "expressions.h"

#define ARR_TREE_RANGE 100000 //< range of tree arrays
#define BUILT_FUNC_NUM 10   //< we have 10 built_in functions
#define NO_ASSIGN 0 //< if expression is not in assignment
#define MAX_FUNCNAME_LEN 100 //< Max function length

#define GENERATE(func) if ((!func)) intern_error()


//// Global parser data ////
typedef struct
{
    Token current_token;
    Token previous_token;
    int current_line;
    TNode *global_table;
    TNode *local_table;
    int index;          //< for searching of retval_arr and arg_arr
    int param_counter;
    char *current_func_id;
    bool current_func_ret_success;
}Metadata;


//// Expression parsing ////
typedef enum
{
    LP, // Left  Para (
    RP, // Right Para )

    ADDS,
    SUBS,
    MULS,
    DIVS,

    GTS, // >
    LTS, // <
    NES, // !=
    LES, // <=
    GES, // >=
    EQS, // ==

    II, // id, float, string, int
    EN, // $
    NT, // Non Terminal
    SH, // SHift
} Terminal_type;

typedef struct
{
    Token token;
    Terminal_type terType;
    Data_type dataType;
    int current_line;

} Terminal;


//// Utility types ////
typedef struct
{
    Data_type id_types[MAX_RET_VAL]; //< IDs on left side of assignment - types
    char* id_names[MAX_RET_VAL];     //< IDs on left side of assignment - identifiers
    char* id_generate[MAX_RET_VAL];  //< IDs for generating
} AssignMetadata;

TData new_data_func, new_data_var;
AssignMetadata asgn_meta;

TData init_new_data(TData new_data);

AssignMetadata init_asgn_data(AssignMetadata asgn_meta);

void check_suspected();

void add_suspected_tree();

void delete_arr_suspected();

bool is_built_fun(char *func_id);

void check_built(TNode *root);

bool search_all_last_trees(char *key);

void program();

bool func();

void header_arg();

void header_ret();

bool func_header(bool *is_main);

void prolog();

bool expect_token(Token_type t_type, Keyword k);

bool statement();

void function_call(Token id, unsigned num_of_id, bool is_built);

void if_s();

void assignment_s(unsigned number_of_id, bool in_for);

void for_s();

void return_s();

bool expr_input(Terminal *input_terminal, bool *func_call, unsigned num_of_id, unsigned *param_num);

Data_type expression(unsigned num_of_id, bool is_bool, bool can_be_empty);

#endif