/**
 * @file codegenerator.c
 * @authors Peter Rucek, Marek Micek, Rebeka Cernianska
 * @date 13 Nov 2020
 * @brief Imlemenatation of code generator
 */

#include "codegenerator.h"
#include "stack.h"

#define MAX_INDEX_LEN 500
#define MAX 100

#define CODE(_code) if(!add_string(&code,(_code)))return false;

// Variadic args MUST be strings (otherwise segfault) & last arg must be "\n" (otherwise infinite cycle)
#define CODELN(...) if(!add_strings(&code, ##__VA_ARGS__)) return false;

#define CODE_INT(_code) \
    do{ \
        char index[MAX_INDEX_LEN];   \
        sprintf(index, "%d", _code);    \
        CODE(index); \
    }while(0)

#define GEN_INT2FLAOT   \
    "LABEL $int2float\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "INT2FLOAT LF@%retval1 LF@%param1\n"    \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

#define GEN_FLOAT2INT   \
    "LABEL $float2int\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "FLOAT2INT LF@%retval1 LF@%param1\n"    \
    "POPFRAME\n" \
    "RETURN\n" \
    "\n"

#define GEN_LEN   \
    "LABEL $len\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "STRLEN LF@%retval1 LF@%param1\n"    \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

#define GEN_INPUTS  \
    "LABEL $inputs\n" \
    "PUSHFRAME\n" \
    "DEFVAR LF@%retval1\n"   \
    "DEFVAR LF@%retval2\n"   \
    "DEFVAR LF@%str_len\n" \
    "DEFVAR LF@%type_check\n" \
    "MOVE LF@%retval2 int@0\n" \
    "READ LF@%retval1 string\n" \
    "TYPE LF@%type_check LF@%retval1\n" \
    "STRLEN LF@%str_len LF@%type_check\n" \
    "JUMPIFNEQ $inputs_end LF@%str_len int@0\n" \
    "JUMPIFEQ $inputs_end LF@type_check string@string\n" \
    "MOVE LF@%retval2 int@1\n" \
    "LABEL $inputs_end\n" \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

#define GEN_INPUTI  \
    "LABEL $inputi\n" \
    "PUSHFRAME\n" \
    "DEFVAR LF@%retval1\n"   \
    "DEFVAR LF@%retval2\n"   \
    "MOVE LF@%retval2 int@0\n" \
    "DEFVAR LF@%str_len\n" \
    "DEFVAR LF@%type_check\n" \
    "READ LF@%retval1 int\n" \
    "TYPE LF@%type_check LF@%retval1\n" \
    "STRLEN LF@%str_len LF@%type_check\n" \
    "JUMPIFNEQ $inputi_end LF@%str_len int@0\n" \
    "JUMPIFEQ $inputi_end LF@type_check string@int\n" \
    "MOVE LF@%retval2 int@1\n" \
    "LABEL $inputi_end\n" \
    "POPFRAME\n" \
    "RETURN\n" \
    "\n"

#define GEN_INPUTF  \
    "LABEL $inputf\n" \
    "PUSHFRAME\n" \
    "DEFVAR LF@%retval1\n"   \
    "DEFVAR LF@%retval2\n"   \
    "MOVE LF@%retval2 int@0\n" \
    "DEFVAR LF@%str_len\n" \
    "DEFVAR LF@%type_check\n" \
    "READ LF@%retval1 float\n" \
    "TYPE LF@%type_check LF@%retval1\n" \
    "STRLEN LF@%str_len LF@%type_check\n" \
    "JUMPIFNEQ $inputf_end LF@%str_len int@0\n" \
    "JUMPIFEQ $inputf_end LF@type_check string@float\n" \
    "MOVE LF@%retval2 int@1\n" \
    "LABEL $inputf_end\n" \
    "POPFRAME\n" \
    "RETURN\n" \
    "\n"

#define GEN_ORD   \
    "LABEL $ord\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "DEFVAR LF@%retval2\n"   \
    "DEFVAR LF@%tmp_val\n"  \
    "DEFVAR LF@%tmp_len\n"  \
    "STRLEN LF@%tmp_len LF@%param1\n"  \
    "MOVE LF@%retval1 int@0\n" \
    "MOVE LF@%retval2 int@0\n" \
    "SUB LF@%retval2 LF@%retval2 int@1\n"  \
    "LT LF@%tmp_val LF@%param2 int@0\n"  \
    "JUMPIFEQ $ord_ret LF@%tmp_val bool@true\n"  \
    "GT LF@%tmp_val LF@%param2 LF@%tmp_len\n"  \
    "JUMPIFEQ $ord_ret LF@%tmp_val bool@true\n"  \
    "STRI2INT LF@%retval1 LF@%param1 LF@%param2\n" \
    "JUMP $ord_end\n" \
    "LABEL $ord_ret\n" \
    "MOVE LF@%retval2 int@1\n" \
    "LABEL $ord_end\n" \
    "POPFRAME\n" \
    "RETURN\n" \
    "\n"

#define GEN_CHR   \
    "LABEL $chr\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "DEFVAR LF@%retval2\n"   \
    "DEFVAR LF@%tmp_index\n"  \
    "MOVE LF@%retval1 int@0\n" \
    "MOVE LF@%retval2 int@0\n" \
    "LT LF@%tmp_index LF@%param1 int@0\n" \
    "JUMPIFEQ $chr_ret LF@%tmp_index bool@true\n" \
    "GT LF@%tmp_index LF@%param1 int@255\n" \
    "JUMPIFEQ $chr_ret LF@%tmp_index bool@true\n" \
    "INT2CHAR LF@%retval1 LF@%param1\n" \
    "JUMP $chr_end\n" \
    "LABEL $chr_ret\n" \
    "MOVE LF@%retval2 int@1\n" \
    "LABEL $chr_end\n"  \
    "POPFRAME\n" \
    "RETURN\n" \
    "\n"

#define GEN_SUBSTR   \
    "LABEL $substr\n"  \
    "PUSHFRAME\n"           \
    "DEFVAR LF@%retval1\n"   \
    "DEFVAR LF@%retval2\n"    \
    "MOVE LF@%retval2 int@0\n"  \
    "MOVE LF@%retval1 string@\n"   \
    "DEFVAR LF@str_len\n"       \
    "DEFVAR LF@condition\n"     \
    "DEFVAR LF@loop_condition\n"    \
    "DEFVAR LF@max_n\n"         \
    "DEFVAR LF@char\n"          \
    "CREATEFRAME\n"             \
    "DEFVAR TF@%param1\n"         \
    "MOVE TF@%param1 LF@%param1\n"    \
    "CALL $len\n"                   \
    "MOVE LF@str_len TF@%retval1\n" \
    "JUMPIFEQ $substr&return LF@str_len int@0\n"    \
    "LT LF@condition LF@%param2 int@0\n"      \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "GT LF@condition LF@%param2 LF@str_len\n"   \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "EQ LF@condition LF@%param2 LF@str_len\n"   \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "LT LF@condition LF@%param3 int@0\n"      \
    "JUMPIFEQ $substr_error LF@condition bool@true\n"   \
    "MOVE LF@max_n LF@str_len\n"    \
    "SUB LF@max_n LF@max_n LF@%param2\n"    \
    "GT LF@condition LF@%param3 LF@max_n\n" \
    "JUMPIFNEQ $substr_loop LF@condition bool@true\n"   \
    "MOVE LF@%param3 LF@max_n\n"    \
    "LABEL $substr_loop\n"          \
    "GETCHAR LF@char LF@%param1 LF@%param2\n"      \
    "CONCAT LF@%retval1 LF@%retval1 LF@char\n"       \
    "ADD LF@%param2 LF@%param2 int@1\n"             \
    "SUB LF@%param3 LF@%param3 int@1\n"             \
    "GT LF@loop_condition LF@%param3 int@0\n"       \
    "JUMPIFEQ $substr_loop LF@loop_condition bool@true\n"   \
    "JUMP $substr&return\n"         \
    "LABEL $substr_error\n"         \
    "MOVE LF@%retval2 int@1\n"      \
    "LABEL $substr&return\n"        \
    "POPFRAME\n" \
    "RETURN\n"  \
    "\n"

dynamic_string code; // code will be stored here and flushed in the end of compilation to stdout if compilation went successful
char *index3;
char *els;
Stack *index_stack;
char *for_index;
char *index2;
Stack *for_index_stack;
char *label_index;

/**
 * @brief Generates all built_in functions
 */
bool gen_built_func()
{
    CODE(GEN_INT2FLAOT);
    CODE(GEN_FLOAT2INT);
    CODE(GEN_LEN);
    CODE(GEN_INPUTS);
    CODE(GEN_INPUTI);
    CODE(GEN_INPUTF);
    CODE(GEN_ORD);
    CODE(GEN_CHR);
    CODE(GEN_SUBSTR);

    return true;
}

/**
 * @brief Creates new TF for params of function before function_call
 */
bool gen_createframe()
{
    CODE("CREATEFRAME\n");
    return true;
}

/**
 * @brief Determines value to be assigned to parameter
 * @param current_token Token which represents current id
 * @return True if code generation successful, otherwise false
 */
bool gen_param_val(Token current_token)
{

    char current_val[MAX_INDEX_LEN];

    char *tmp = malloc(1000);
    for (int i = 0; i < 1000; i++)
        tmp[i] = 0;

    switch (current_token.type)
    {
        case INT:
            sprintf(current_val, "%d", current_token.data.i);
            CODELN("int@", current_val, "\n");
            break;

         case FLOAT64:
            sprintf(current_val, "%a", current_token.data.d);
            CODELN("float@", current_val, "\n");
            break;

        case STRING:
            
            str2our_str(tmp,current_token.data.s);
            CODELN("string@", tmp, "\n");
            free(tmp);
            break;

        case ID:
            if (current_token.data.s[0] == '%')
            {
                CODE("LF@"); CODE(current_token.data.s); CODE("\n");
            }
            else
            {
                CODE("LF@"); CODE(current_token.data.s); CODE_INT(var_deep); CODE("\n");
            }
            break;
        default:
            break;
    }
    return true;
}

/**
 * @brief Generate needed header and alloc code buffer
 */
bool gen_header()
{
    dynstr_init(&code);

    CODE(".IFJcode20\n");
    index3 = malloc(sizeof(char));
    index2 = malloc(sizeof(char));
    els = malloc(sizeof(char));
    for_index = malloc(sizeof(char));
    label_index = malloc(sizeof(char));
    if (index3 == NULL || els == NULL || for_index == NULL || index2 == NULL || label_index == NULL)
    {
        intern_error();
    }
    *index3 = 64;
    *index2 = 64;
    *label_index = 64;
    index_stack = createStack(100);
    for_index_stack = createStack(100);

    // constants needed, to add
    CODE("DEFVAR GF@_\n");
    CODE("DEFVAR GF@expr_result\n");
    CODE("DEFVAR GF@tmp1\n");
    CODE("DEFVAR GF@tmp2\n");
    CODE("DEFVAR GF@iteration\n");
    CODE("DEFVAR GF@tmp_iteration\n");
    CODE("DEFVAR GF@iter_condition\n");
    CODE("MOVE GF@expr_result bool@true\n");
    CODE("MOVE GF@iteration int@0\n");
    CODE("JUMP $main\n");
    CODE("\n");

    return true;
}

/**
 * @brief Generates beginning of main scope
 * @return True if code generation successful, otherwise false
 */
bool gen_main_start()
{
    CODE("LABEL $main\n");
    CODE(" CREATEFRAME\n");
    CODE(" PUSHFRAME\n");
    return true;
}

/**
 * @brief Generate function header
 */
bool gen_func_header(char *func_name)
{
    CODELN("LABEL $", func_name, "\n");
    CODE(" PUSHFRAME\n");
    return true;
}

/**
 * @brief Generates definition of variables which will store ret value of function
 * @param ret_counter Determines how many ret values we will need
 * @return True if code generation successful, otherwise false
 */
bool gen_func_retval(unsigned ret_counter)
{
    for (unsigned i = 0; i < ret_counter; i++)
    {
        CODE(" DEFVAR LF@%retval"); CODE_INT(i+1); CODE("\n");
    }
    return true;
}

/**
 * @brief Generates assignment of expr_result to retval of function
 * @param ret_counter Determines how many ret values we will need
 * @return True if code generation successful, otherwise false
 */
bool gen_func_return(char *func_id, unsigned ret_counter)
{
    for (unsigned i = 0; i < ret_counter; i++)
    {
        CODE("  POPS GF@expr_result\n");
        CODE(" MOVE LF@%retval"); CODE_INT(i+1); CODE(" GF@expr_result\n");
    }
    CODELN("JUMP $", func_id, "&return", "\n");
    return true;
}

/**
 * @brief Generates assignment of return values to vars on left side
 * @param id Id of var on left side
 * @param retval_index Index of return value
 * @return True if code generation successful, otherwise false
 */
bool gen_retval_assign(char *id, unsigned retval_index, bool is__)
{
    if (is__)
    {
        CODE(" MOVE GF@_"); CODE(" TF@%retval"); CODE_INT(retval_index); CODE("\n");
    }
    else
    {
        if (id[0] == '%')
        {
            CODE(" MOVE LF@"); CODE(id); CODE(" TF@%retval"); CODE_INT(retval_index); CODE("\n");
        }
        else
        {
            CODE(" MOVE LF@"); CODE(id); CODE_INT(var_deep); CODE(" TF@%retval"); CODE_INT(retval_index); CODE("\n");
        }
    }
    return true;
}

/**
 * @brief Generates calling of function
 * @param func_id Stores id of function to be called
 * @return True if code generation successful, otherwise false
 */
bool gen_func_call(char *func_id)
{
    CODELN(" CALL $", func_id, "\n");
    return true;
}

/**
 * @brief Generates passing of values to params in function
 * @param current_token Token which represents current parameter
 * @param param_index Stores index of parameter
 * @return True if code generation successful, otherwise false
 */
bool gen_param_pass(Token current_token, int param_index)
{
    CODE(" DEFVAR TF@%param"); CODE_INT(param_index); CODE("\n");
    CODE(" MOVE TF@%param"); CODE_INT(param_index); CODE(" ");
    if (!gen_param_val(current_token))
    {
        return false;
    }
    return true;
}

/**
 * @brief Genrates build_in function print
 */
bool gen_print(Token current_token, unsigned act_param_counter)
{
    if (act_param_counter == 1) CODE("DEFVAR TF@%param\n");
    CODE("MOVE TF@%param ");
    if (!gen_param_val(current_token)) return false;
    CODE("WRITE TF@%param\n");

    return true;
}

/**
 * @brief Generate if branch
 */
bool if_label()
{
    (*index3)++;
    push(index_stack, *index3);
    CODELN("LABEL $if", index3, "\n");
    return true;
}

/**
 * @brief Generate jump over else branch to end of if
 */
bool if_jump()
{
    *els = top(index_stack);
    CODELN("JUMP $if_end", els, "\n");
    return true;
}

/**
 * @brief Generate jump over if branch to else
 */
bool else_jump()
{
    CODE("  POPS GF@expr_result\n");
    CODELN("JUMPIFEQ $else", index3, " GF@expr_result bool@false", "\n");
    return true;
}

/**
 * @brief Generate else branch
 */
bool else_label()
{
    *els = top(index_stack);
    CODELN("LABEL $else", els, "\n");
    return true;
}

/**
 * @brief Generate end of if
 */
bool if_end_label()
{
    *els = top(index_stack);
    CODELN("LABEL $if_end", els, "\n");
    pop(index_stack);
    return true;
}

/**
 * @brief Generate for header
 */
bool for_header()
{
    (*index2)++;
    push(for_index_stack, *index2);
    *for_index = top(for_index_stack);
    CODELN("LABEL $for", for_index, "\n");
    CODELN("LABEL $condition", for_index, "\n");
    return true;
}


/**
 * @brief Generate for header
 */
bool for_condition_eval()
{
    *for_index = top(for_index_stack);
    CODE("  POPS GF@expr_result\n");
    CODELN("JUMPIFEQ $end_for", for_index," GF@expr_result bool@false", "\n");
    CODE("ADD GF@iteration GF@iteration int@1\n");
    CODELN("JUMP $for_body", for_index, "\n");
    CODELN("LABEL $increment", for_index, "\n");
    return true;
}

/**
 * @brief Generate for header
 */
bool for_body()
{
    *for_index = top(for_index_stack);
    CODELN("JUMP $condition", for_index, "\n");
    CODELN("LABEL $for_body", for_index, "\n");
    return true;
}

/**
 * @brief Generate for header
 */
bool for_end()
{
    *for_index = top(for_index_stack);
    CODELN("JUMP $increment", for_index, "\n");
    CODELN("LABEL $end_for", for_index, "\n");
    pop(for_index_stack);
    return true;
}

/**
 * @brief Stores current iter value before entering nested for
 */
bool gen_start_nested_for()
{
    (*label_index)++;
    CODE("MOVE GF@tmp_iteration GF@iteration\n");
    CODE("GT GF@iter_condition GF@iteration int@1\n");
    CODELN("JUMPIFEQ $jump_iter_assign", label_index ," GF@iter_condition bool@true", "\n");
    
    CODE("MOVE GF@iteration int@0\n"); 
    CODELN("LABEL $jump_iter_assign", label_index, "\n");
    return true;
}

/**
 * @brief Restores the old iter value of parent for
 */
bool gen_end_nested_for()
{
    CODE("MOVE GF@iteration GF@tmp_iteration\n");
    return true;
}

// sets global iterator to zero when out of last for in current scope
bool gen_clear_iter()
{
    CODE("MOVE GF@iteration int@0\n");
    return true;
}

/**
 * @brief Generates jump to define var only in first iteration of for loop
 */
bool gen_jump_def(char *var_id)
{
    CODE("GT GF@iter_condition GF@iteration int@1\n");    //< jump over definition when in bigger iteration than first
    if (!is_else)
    {
        CODE("JUMPIFEQ $"); CODE(var_id); CODE_INT(var_deep); CODE("&redefinition"); CODE(" GF@iter_condition bool@true\n"); 
    }
    else
    {
        CODE("JUMPIFEQ $else_"); CODE(var_id); CODE_INT(var_deep); CODE("&redefinition"); CODE(" GF@iter_condition bool@true\n"); 
    }
    return true;  
}

/**
 * @brief Special function for definition of var in for loop to avoid interpret redefinition
 */
bool gen_var_in_for(char *var_id)
{
    CODE(" DEFVAR LF@"); CODE(var_id); CODE_INT(var_deep); CODE("\n");
    if (!is_else)
    {
        CODE("LABEL $"); CODE(var_id); CODE_INT(var_deep); CODE("&redefinition\n");
    }
    else
    {
        CODE("LABEL $else_"); CODE(var_id); CODE_INT(var_deep); CODE("&redefinition\n");   
    }
    
    CODE("  POPS GF@expr_result\n");
    if(var_id[0] == '%')
    {
        CODE(" MOVE LF@"); CODE(var_id); CODE(" GF@expr_result\n");
    }
    else
    {
        CODE(" MOVE LF@"); CODE(var_id); CODE_INT(var_deep); CODE(" GF@expr_result\n");
    }
    
    
    return true;
}

/**
 * @brief Defines new var in LF
 * @param id Id of new var
 * @return True if generation successful, else false
 */
bool gen_var_def(char *id)
{
    CODE(" DEFVAR LF@"); CODE(id); CODE_INT(var_deep); CODE("\n");
    CODE("  POPS GF@expr_result\n");
    CODE(" MOVE LF@"); CODE(id); CODE_INT(var_deep); CODE(" GF@expr_result\n");
    return true;
}

bool gen_var_ass(char *id, bool is__)
{
    if (is__)
    {
        CODE("  POPS GF@expr_result\n");
        CODELN(" MOVE GF@_ GF@expr_result", "\n");
    }
    else
    {
        CODE("  POPS GF@expr_result\n");
        if(id[0] == '%')
        {
            CODE(" MOVE LF@"); CODE(id); CODE(" GF@expr_result\n");
        }
        else
        {
            CODE(" MOVE LF@"); CODE(id); CODE_INT(var_deep); CODE(" GF@expr_result\n");
        }
    }
    return true;
}

/**
 * @brief Generate main end
 */
bool gen_main_end()
{
    CODE("LABEL $main&return\n");
    CODE("POPFRAME\n");
    CODE("CLEARS\n");
    CODE("JUMP $end_of_code\n");
    CODE("\n");
    return true;
}

/**
 * @brief Generate function end
 */
bool gen_func_end(char *func_id)
{
    CODELN("LABEL $", func_id, "&return", "\n");
    CODE("POPFRAME\n");
    CODE("RETURN\n");
    CODE("\n");

    return true;
}

/**
 * @brief Generate label where to jump after main end, so other functions are not executed
 */
bool gen_code_end()
{
    CODE("LABEL $end_of_code\n");
    return true;
}

/**
 * @brief casts double number @param d into hex string @param buf
 */
void float2hex(double d, char *buf)
{
    sprintf(buf, "%a", d);
}

/**
 * @brief casts integer number @param n into string @param buf
 */
void int2str(int n, char *buf)
{
    sprintf(buf, "%d", n);
}

/**
 * @brief edits @param src string into @param dst string which is needed in 3AC
 */
void str2our_str(char *dst, char *src)
{
    unsigned j = 0;
    bool escape = false;
    for (unsigned i = 1; i < strlen(src)-1; i++, j++)
    {
        if ((src[i] >= 0 && src[i] <= 32 )|| src[i] == 34 || src[i] == 35 || src[i] == 92)
        {
            if (escape)// \", \\ ,
            {
                escape = false;
                if (src[i] == '\\') src[i] = '\\';
                if (src[i] == '\"') src[i] = '\"';
            }
            else if (src[i] == 92)
            {
                escape = true;
                j--;
                continue;
            }

            char buf[3];
            dst[j++] = '\\'; dst[j++] = '0';
            int2str(src[i],buf);
            if(strlen(buf) == 1)
            {
                dst[j++] = '0';
                dst[j] = buf[0];
            }
            if(strlen(buf) == 2)
            {
                dst[j++] = buf[0];
                dst[j] = buf[1];
            }
        }
        else if (escape) // \n, \t
        {
            escape = false;
            if (src[i] == 'n') src[i] = '\n';
            if (src[i] == 't') src[i] = '\t';
            if (src[i] == 'x') // \xhh
            {
                char buf[3];
                buf[0] = src[++i];
                buf[1] = src[++i];
                int num = (int)strtol(buf, NULL, 16); // from hex to int
                int2str(num,buf);
                dst[j++] = '\\';
                if(strlen(buf) == 1)
                {
                    dst[j++] = '0';
                    dst[j++] = '0';
                    dst[j] = buf[0];
                }
                if(strlen(buf) == 2)
                {
                    dst[j++] = '0';
                    dst[j++] = buf[0];
                    dst[j] = buf[1];
                }
                if(strlen(buf) == 3)
                {
                    dst[j++] = buf[0];
                    dst[j++] = buf[1];
                    dst[j] = buf[2];
                }
                continue;
            }
            char buf[3];
            dst[j++] = '\\'; dst[j++] = '0';
            int2str(src[i],buf);
            if(strlen(buf) == 1)
            {
                dst[j++] = '0';
                dst[j] = buf[0];
            }
            if(strlen(buf) == 2)
            {
                dst[j++] = buf[0];
                dst[j] = buf[1];
            }
        }
        else
        {
            dst[j] = src[i];
        }
    }
}

// for debugging
bool gen_expr_begin()
{
    CODE("# EXPR BEGIN\n")
    return true;
}

// for debugging
bool gen_expr_end()
{
    CODE("# EXPR END\n");
    return true;
}

/**
 * @brief Generates term(string, int, float64, ID)
 * @param type is type of term in string
 * @param constant is the data/ value of term
 */
bool gen_term(char *type, char* constant)
{
    CODELN("  PUSHS ",type,"@",constant,"\n");
    return true;
}

/**
 * @brief Generates operation to be performed on stack
 * @param type should be Terminal_type, but codegenerator.h would have needed parser.h
 * @param type is enum of the operation
 */
bool gen_operation(int type, bool concat, bool idiv)
{
    if (type == 2)
    {
        if (concat)
        {
            CODE("  POPS GF@tmp2\n");
            CODE("  POPS GF@tmp1\n");
            CODE("  CONCAT GF@expr_result GF@tmp1 GF@tmp2\n");
            CODE("  PUSHS GF@expr_result\n");
        }
        else
            CODE("  ADDS\n");
    }
    if (type == 3)
        CODE("  SUBS\n");
    if (type == 4)
        CODE("  MULS\n");
    if (type == 5)
    {
        if (idiv)
        {
            CODE("  IDIVS\n");
        }
        else
            CODE("  DIVS\n");
    }
    if (type == 6)
        CODE("  GTS\n");
    if (type == 7)
        CODE("  LTS\n");
    if (type == 8)
        CODE("  EQS\nNOTS\n");
    if (type == 9) //LES
    {
        CODE("  POPS GF@tmp1\n");
        CODE("  POPS GF@tmp2\n");
        CODE("  PUSHS GF@tmp1\n");
        CODE("  PUSHS GF@tmp2\n");
        CODE("  LTS\n");
        CODE("  PUSHS GF@tmp1\n");
        CODE("  PUSHS GF@tmp2\n");
        CODE("  EQS\n");
        CODE("  ORS\n");
    }

    if (type == 10) //GES
    {
        CODE("  POPS GF@tmp1\n");
        CODE("  POPS GF@tmp2\n");
        CODE("  PUSHS GF@tmp1\n");
        CODE("  PUSHS GF@tmp2\n");
        CODE("  GTS\n");
        CODE("  PUSHS GF@tmp1\n");
        CODE("  PUSHS GF@tmp2\n");
        CODE("  EQS\n");
        CODE("  ORS\n");
    }
    if (type == 11)
        CODE("  EQS\n");

    return true;
}


/**
 * @brief free allocated code buffers
 */
void gen_dispose()
{
    dynstr_free(&code);
    free(index3);
    free(index2);
    free(els);
    free(for_index);
    free(label_index);
}

/**
 * @brief Puts code buffer to stdout
 */
void flush()
{
    puts(code.buff);
    gen_dispose();
}