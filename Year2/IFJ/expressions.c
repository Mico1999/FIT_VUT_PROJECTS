/**
 * @file expressions.c
 * @authors Peter Rucek
 * @date 16 Nov 2020
 * @brief Imlemenatation of expressions
 */

#include "expressions.h"

    // LP, // Left  Para (
    // RP, // Right Para )

    // ADDS,
    // SUBS,
    // MULS,
    // DIVS,

    // GTS, // >
    // LTS, // <
    // NES, // !=
    // LES, // <=
    // GES, // >=
    // EQS, // ==

    // II, // id, float, string, int
    // EN, // $

TabItem precedence_table[TABLE_SIZE][TABLE_SIZE] =
{ //||_(__||_)__||_+_||__-_||_*__||_/__||__>__||__<_||_!=_||_<=_||_>=_||_==_||_i__||_$__||
    {SHIFT,EQUAL,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR}, // (
    {T_ERR,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,T_ERR,REDUC}, // )
    {SHIFT,REDUC,REDUC,REDUC,SHIFT,SHIFT,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,SHIFT,REDUC}, // +
    {SHIFT,REDUC,REDUC,REDUC,SHIFT,SHIFT,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,SHIFT,REDUC}, // -
    {SHIFT,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,SHIFT,REDUC}, // *
    {SHIFT,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,SHIFT,REDUC}, // /
    {SHIFT,REDUC,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,SHIFT,REDUC}, // >
    {SHIFT,REDUC,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,SHIFT,REDUC}, // <
    {SHIFT,REDUC,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,SHIFT,REDUC}, // !=
    {SHIFT,REDUC,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,SHIFT,REDUC}, // <=
    {SHIFT,REDUC,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,SHIFT,REDUC}, // >=
    {SHIFT,REDUC,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,T_ERR,SHIFT,REDUC}, // ==
    {T_ERR,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,REDUC,T_ERR,REDUC}, // i
    {SHIFT,T_ERR,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,SHIFT,T_ERR}  // $
                                                                                 //^^^ only ok variation
};

Data_type curr_data_type = T_UNDEFINED;

// for debugging
void printStack(Stack *s)
{
    for(int i = 0; i < 10; i++)
        printf("%d |",s->array[i]);
}

/**
 * @brief Adding tokens to stack an selecting action to be done by precedence table
 */
bool expr(Data_type *expr_type, bool *func_call, unsigned num_of_id, bool is_bool, bool can_be_empty)
{
    Terminal input_terminal;
    bool expr_isbool = false;       //< Decide wheater exrpessions is type of bool
    bool expr_semerror = false;     //< Wait for data type collisions or undefined IDs and if any occur, be sure to return false
    bool can_be_func = false;       //< Will not print error if ID is not defined, can still be a func call, and if not then print error
    bool check_zero_div = false;
    Terminal previous;
    int num_of_reads = 0;
    int begin_counter = 0;
    unsigned num_of_para = 0;

    Stack *s = createStack(STACK_SIZE);
    push(s,EN);

    bool read = true;

    while(true)
    {
        if (read)
        {
            if (!expr_input(&input_terminal,func_call,num_of_id,&num_of_para))
            {
                deleteStack(s);
                return false;
            }
            else
            {
                num_of_reads++;

                if (can_be_func) // id was read without data type
                {
                    if (input_terminal.terType == LP && *func_call)
                    {
                        deleteStack(s);
                        return true;
                    }
                    else
                    {
                        if (strcmp(previous.token.data.s, "_"))
                        {
                            no_definition_error(previous.token.data.s, previous.current_line);
                        }
                        else
                        {
                            other_error(previous.current_line);
                        }
                        //free(previous.token.data.s);
                        if (!expr_semerror) expr_semerror = true;
                    }
                    can_be_func = false;
                }

                if (input_terminal.terType == II) //< Operand
                {
                    if (check_zero_div)
                    {
                        if (input_terminal.token.data.i == 0 || input_terminal.token.data.d == (double)0)
                        {
                            div_zero_error(input_terminal.current_line);
                        }
                        check_zero_div = false;
                    }
                    // Check terminal's data type
                    if (*expr_type == T_UNDEFINED) //< Still not determined expr type
                    {
                        if (input_terminal.dataType != T_UNDEFINED)
                        {
                            *expr_type = input_terminal.dataType;
                            curr_data_type = *expr_type;
                        }
                            
                        else 
                        {
                            // can be error or func call
                            can_be_func = true;
                            previous = input_terminal;
                        }
                    }
                    else
                    {
                        if (input_terminal.dataType != *expr_type)
                        {
                            // Invalid operand types
                            if (input_terminal.dataType != T_UNDEFINED)
                            {
                                compatibility_error(data_types[*expr_type], input_terminal.current_line);
                                if (!expr_semerror) expr_semerror = true;    
                            }
                        }
                    }

                    if (begin_counter == 0)
                    {
                        GENERATE(gen_expr_begin());
                        begin_counter++;
                    }

                    char *type = malloc(MAX_STRING);
                    for (int i = 0; i < MAX_STRING; i++)
                        type[i] = 0;
                    char *data = malloc(MAX_STRING);
                    for (int i = 0; i < MAX_STRING; i++)
                        data[i] = 0;
                    
                    if (input_terminal.token.type == STRING)
                    {
                        strcpy(type,"string");
                        str2our_str(data,input_terminal.token.data.s);
                        GENERATE(gen_term(type,data));
                    }
                    if (input_terminal.token.type == INT)
                    {
                        strcpy(type,"int");
                        int2str(input_terminal.token.data.i,data);
                        GENERATE(gen_term(type,data));
                    }
                    if (input_terminal.token.type == FLOAT64)
                    {
                        strcpy(type,"float");
                        float2hex(input_terminal.token.data.d,data);
                        GENERATE(gen_term(type,data));
                    }
                    if (input_terminal.token.type == ID && !can_be_func)
                    {
                        strcpy(type,"LF");
                        char buf[2];

                        if (num_of_para != 0)
                        {
                            strcpy(data,"%param");
                            int2str(num_of_para,buf);
                            strcat(data,buf);
                            num_of_para = 0;
                        }
                        else
                        {
                            strcpy(data,input_terminal.token.data.s);
                            int2str(var_deep, buf);
                            strcat(data, buf);
                        }
                        GENERATE(gen_term(type,data));
                    }

                    free(type);
                    free(data);

                    /** @todo Free if id came */
                    if (input_terminal.dataType == T_STRING) // || input_terminal.token.type == ID)   not working, invalid read
                    {
                        //free(input_terminal.token.data.s); //< Stop holding on to IDs
                        input_terminal.token.data.s = NULL;
                    }
                }
                if (*expr_type == T_STRING)
                {
                    if (input_terminal.terType == SUBS || input_terminal.terType == MULS || input_terminal.terType == DIVS)
                    {
                        compatibility_error(data_types[*expr_type], input_terminal.current_line);
                        if (!expr_semerror) expr_semerror = true;
                    }
                }
                if (input_terminal.terType == DIVS)
                {
                    check_zero_div = true;
                }
                if (input_terminal.terType == GTS || input_terminal.terType == NES || input_terminal.terType == GES||
                    input_terminal.terType == LTS || input_terminal.terType == EQS || input_terminal.terType == LES ) //< Logical Operator
                {
                    if (!expr_isbool) expr_isbool = true;
                }
            }
        }

        int f = find_terminal(s);
        if (f < 0 || input_terminal.terType < 0)
        {
            deleteStack(s);
            return false;
        }
        TabItem to_perform = precedence_table[f][input_terminal.terType];

        if (to_perform == T_ERR)
        {
            deleteStack(s);
            if (input_terminal.terType == EN && f == EN) // input token == first terminal on stack == EN
            {
                if (expr_semerror)
                {
                    return false;
                }
                if (expr_isbool == is_bool)
                {
                    if (num_of_reads > 1 || can_be_empty) // || previous.terType == EN  ?
                    {
                        if(!can_be_empty)
                            GENERATE(gen_expr_end());
                        return true;
                    }
                    else return false;
                }
                else
                {
                    if (num_of_reads > 1)
                        compatibility_error(data_types[*expr_type], input_terminal.current_line);
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else if (to_perform == REDUC)
        {
            if (!reduce(s))
            {
                return false;
            }
            read = false;
            continue;
        }
        else if (to_perform == SHIFT)
        {
            if (input_terminal.terType == II)
            {
                push(s,SH);
            }
            else if (input_terminal.terType != LP && input_terminal.terType != RP &&
                     input_terminal.terType != II && input_terminal.terType != EN  )
            {
                int tmp = pop(s);
                push(s,SH);
                push(s,tmp);
            }
            push(s,input_terminal.terType);
            read = true;
            continue;
        }
        else if (to_perform == EQUAL)
        {
            push(s,input_terminal.terType);
            if (!reduce(s))
            {
                deleteStack(s);
                return false;
            }
            read = true;
            continue;
        }
    }
}

/**
 * @brief Find first terminal in stack
 */
int find_terminal(Stack *s)
{
    for(int i = s->top; i > -1; i--)
    {
        if(s->array[i] != NT)
        {
            return s->array[i];
        }
    }
    return -1;
}

/**
 * @brief Reduce what is on stack
 */
bool reduce(Stack *s)
{
    int token[STACK_SIZE] = {0};
    int i = -1;
    do
    {
        i++;
        if (top(s) == STACK_ERROR)
        {
            return false;
        }
        token[i] = pop(s);
    } while ( token[i] != SH && token[i] != LP );

    // implmented rules
    if((token[0] == II && token[1] == SH) ||
       (token[0] == RP && token[1] == NT   && token[2] == LP ) ||
       (token[0] == NT && token[1] == ADDS && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == SUBS && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == MULS && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == DIVS && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == NES  && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == EQS  && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == GES  && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == LES  && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == LTS  && token[2] == NT && token[3] == SH) ||
       (token[0] == NT && token[1] == GTS  && token[2] == NT && token[3] == SH)   )
    {
        push(s,NT);

        bool concat = false;
        bool idiv = false;

        if (token[1] == ADDS && curr_data_type == T_STRING)
        {
            concat = true;
        }
        if (token[1] == DIVS && curr_data_type == T_INT)
        {
            idiv = true;
        }
        if (token[1] != NT && token[1] != SH)
            GENERATE(gen_operation(token[1],concat,idiv));
        return true;
    }
    else
    {
        return false;
    }
}