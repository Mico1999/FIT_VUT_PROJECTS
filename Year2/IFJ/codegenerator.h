/**
 * @file codegenerator.h
 * @authors Peter Rucek, Marek Micek, Rebeka Cernianska
 * @date 13 Nov 2020
 * @brief Inteface for codegenerator.c
 */

#ifndef _CODEGENERATOR_H_
#define _CODEGENERATOR_H_

#include "dynamicstring.h"
#include "symtable.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

extern int var_deep;
extern bool is_else;
extern bool is_in_for;

bool gen_jump_def(char *var_id);

bool gen_var_in_for(char *var_id);

bool gen_start_nested_for();

bool gen_end_nested_for();

bool gen_clear_iter();

bool gen_built_func();

bool gen_header();

void gen_dispose();

bool gen_main_start();

bool gen_func_header(char *func_name);

bool gen_func_call(char *func_id);

bool gen_func_retval(unsigned ret_counter);

bool gen_func_return(char *func_id, unsigned ret_counter);

bool gen_retval_assign(char *id, unsigned retval_index, bool is__);

bool gen_param_pass(Token current_token, int param_index);

bool gen_param_val(Token current_token);

bool gen_print(Token current_token, unsigned act_param_counter);

bool gen_var_def(char *id);

bool gen_var_ass(char *id, bool is__);

bool gen_createframe();

bool gen_main_end();

bool gen_inputs();

bool gen_term(char *type, char* constant);

void float2hex(double d, char *buf);

void int2str(int n, char *buf);

void str2our_str(char *dst, char *src);

bool gen_operation(int type, bool concat, bool idiv);

bool gen_expr_result();

bool gen_expr_begin();

bool gen_expr_end();

bool if_label();

bool if_jump();

bool else_jump();

bool else_label();

bool if_end_label();

bool for_header();

bool for_condition_eval();

bool for_body();

bool for_end();

bool gen_code_end();

bool gen_func_end(char *func_id);

void flush();

#endif