/**
 * @file parser.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Implementation
 */

#include "parser.h"

#define GET_TOKEN()  \
        do{ m.current_token = get_next_token(stdin); \
            if (inc_line_on_next){ m.current_line++; inc_line_on_next=false;}\
            if (m.current_token.type == EOL) inc_line_on_next = true; \
            (m.current_token.type == ERROR)? (lexical_error(m.current_line), 1) : 0 ;\
        }while(0)

#define CHECK_TOKEN(Type) \
        ((m.current_token.type == (Type))? true : \
        (syntax_error(m.current_token.type,m.current_line), \
        ((m.current_token.type == ID || m.current_token.type == STRING)? \
        free(m.current_token.data.s),false : false), false))

#define CHECK_TOKEN_NOFREE(Type) \
        ((m.current_token.type == (Type))? true : \
        (syntax_error(m.current_token.type,m.current_line), false))

#define CHECK_NO_ERROR(Type) ((m.current_token.type == (Type))? true : false)

#define GET_AND_CHECK(Type) do {GET_TOKEN(); CHECK_TOKEN((Type));}while(0)

#define IS_DATA_TYPE() \
        (m.current_token.type == KEYWORD && \
        (m.current_token.data.k == K_STRING || \
         m.current_token.data.k == K_INT || \
         m.current_token.data.k == K_FLOAT64))? \
         true : (syntax_error(m.current_token.type,m.current_line),false)

#define IS_EXPR_END(toktype) \
    (  (toktype) == EOL      \
    || (toktype) == COMMA    \
    || (toktype) == SEMICLN  \
    || (toktype) == BRACKET_LEFT \
    || (toktype) == EoF )

#define CONSUME_FAILED_EXPR() \
        while (!IS_EXPR_END(m.current_token.type)) \
        { \
            GET_TOKEN(); \
        }

#define CONSUME_LINE() \
        while (!CHECK_NO_ERROR(EOL) && !CHECK_NO_ERROR(EoF)) \
        { \
            GET_TOKEN(); \
        }


//global
bool inc_line_on_next = false;
bool else_shadow = false;
bool is_in_nested_for = false;

Metadata m = {
    .current_line = 1,
    .index = 0,
    .local_table = NULL,
    .current_func_id = NULL,
    .current_func_ret_success = false,
    .param_counter = 0
};

TNode *node;

TNode *array_of_trees[ARR_TREE_RANGE];      //< stores all blocks with their variables + params of current func
int tree_index = -1;

TNode *arr_suspected[ARR_TREE_RANGE];       //< stores all funcs suspected from no_definition
int suspected_tree_idx = -1;

char *built_in[] = {"inputs", "inputf", "inputi", "print", "int2float", "float2int",
"len", "substr", "ord", "chr"};

//extern vars of code_generator
int var_deep = 0;
bool is_else = false;
bool is_in_for = false;

/**
 * @brief Inits data structure of symtable to eliminate multi. insertion of one symbol
 * @param new_data Data unit that will be initionalized
 * @return Initionalized data ready for next use
 */
TData init_new_data(TData new_data)
{
    new_data.type = T_UNDEFINED;
    new_data.defined = new_data.is_var = new_data.is_function = new_data.in_block = new_data.is_param = false;
    new_data.param_counter = new_data.ret_counter = new_data.line = new_data.id_counter = new_data.param_num = 0;

    for (int i = 0; i < MAX_RET_VAL; i++)
    {
        new_data.retval_arr[i] = T_UNDEFINED;
        new_data.id_type[i] = T_UNDEFINED;
    }
    for (int j = 0; j < MAX_ARG; j++)
        new_data.arg_arr[j] = T_UNDEFINED;

    return new_data;
}

/**
 * @brief Inits new instance of asgn_meta
 * @param asgn_meta Current metadata that will be initionalized
 * @return Initionalized metadata ready for next use
 */
 AssignMetadata init_asgn_data(AssignMetadata asgn_meta)
{
    for (int i = 0; i < MAX_RET_VAL; i++)
        asgn_meta.id_types[i] = T_UNDEFINED;

    for (int j = 0; j < MAX_RET_VAL; j++)
        asgn_meta.id_names[j] = "";

     for (int j = 0; j < MAX_RET_VAL; j++)
        asgn_meta.id_generate[j] = "";

    return asgn_meta;
}

/**
 * @brief Inits another tree in array of suspected tree
 */
void add_suspected_tree()
{
    suspected_tree_idx++;
    if (suspected_tree_idx < ARR_TREE_RANGE)
        arr_suspected[suspected_tree_idx] = init_symtable(arr_suspected[suspected_tree_idx]);
}

/**
 * @brief Deletes the whole array of suspected trees
 */
void delete_arr_suspected()
{
    for (int i = 0; i <= suspected_tree_idx; i++)
    {
        delete_symtable(arr_suspected[i]);
    }
}

/**
 * @brief Checks semantics of funcs suspected from no_definition
 */
void check_suspected()
{
    for (int idx = 0; idx <= suspected_tree_idx; idx++)
    {
        TNode *root = arr_suspected[idx];

        if ((node = search_symtable(m.global_table, root->key)) == NULL)    //< func is not defined
        {
            no_definition_error(root->key, root->data.line);
        }
        else  //< func is defined, check params
        {
            if (node->data.param_counter != root->data.param_counter)
            {
                param_num_error(root->key, root->data.line);
            }
            else
            {
                for (unsigned i = 0; i < node->data.param_counter; i++)
                {
                    if (node->data.arg_arr[i] != root->data.arg_arr[i])     //< data type not same
                    {
                        param_type_error(root->key, root->data.line);
                        break;
                    }
                }
            }
            // check semantics of left side of var assignment
            if (node->data.ret_counter != root->data.id_counter)
            {
                return_unpack_error(root->key, root->data.line);
            }
            else
            {
                for (unsigned idx = 0; idx < node->data.ret_counter; idx++)
                {
                    if (node->data.retval_arr[idx] != root->data.id_type[idx])  //< unmatched data type of left id and ret value
                    {
                        return_type_error(root->key, root->data.line);
                        break;
                    }
                }
            }
       }
    }
}

/**
 * @brief Checks semantics of buil_in functions
 * @param root Root of global symtable
*/
void check_built(TNode *root)
{
    if (root != NULL)
    {
        //using PostOrder
        check_built(root->lptr);
        check_built(root->rptr);

        bool is_built = is_built_fun(root->key);    //< whether current func is built_in
        if (is_built)
        {
            if (!strcmp(root->key, "inputs"))
            {
                if (root->data.param_counter != 0 || root->data.id_counter != 2)
                {
                    param_error(root->key, root->data.line);
                }
                else if ((root->data.id_type[0] != T_STRING && root->data.id_type[0] != T_UNDEFINED) || (root->data.id_type[1] != T_INT && root->data.id_type[1] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "inputi"))
            {
                if (root->data.param_counter != 0 || root->data.id_counter != 2)
                {
                    param_error(root->key, root->data.line);
                }
                else if ((root->data.id_type[0] != T_INT && root->data.id_type[0] != T_UNDEFINED) || (root->data.id_type[1] != T_INT && root->data.id_type[1] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "inputf"))
            {
                if (root->data.param_counter != 0 || root->data.id_counter != 2)
                {
                    param_error(root->key, root->data.line);
                }
                else if ((root->data.id_type[0] != T_FLOAT64 && root->data.id_type[0] != T_UNDEFINED) || (root->data.id_type[1] != T_INT && root->data.id_type[1] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "int2float"))
            {
                if (root->data.param_counter != 1 || root->data.id_counter != 1)
                {
                    param_error(root->key, root->data.line);
                }
                else if (root->data.arg_arr[0] != T_INT || (root->data.id_type[0] != T_FLOAT64 && root->data.id_type[0] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "float2int"))
            {
                if (root->data.param_counter != 1 || root->data.id_counter != 1)
                {
                    param_error(root->key, root->data.line);
                }
                else if (root->data.arg_arr[0] != T_FLOAT64 || (root->data.id_type[0] != T_INT && root->data.id_type[0] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "len"))
            {
                if (root->data.param_counter != 1 || root->data.id_counter != 1)
                {
                    param_error(root->key, root->data.line);
                }
                else if (root->data.arg_arr[0] != T_STRING || (root->data.id_type[0] != T_INT && root->data.id_type[0] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "substr"))
            {
                if (root->data.param_counter != 3 || root->data.id_counter != 2)
                {
                    param_error(root->key, root->data.line);
                }
                else if (root->data.arg_arr[0] != T_STRING || root->data.arg_arr[1] != T_INT || root->data.arg_arr[2] != T_INT || (root->data.id_type[0] != T_STRING && root->data.id_type[0] != T_UNDEFINED) || (root->data.id_type[1] != T_INT && root->data.id_type[1] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "ord"))
            {
                if (root->data.param_counter != 2 || root->data.id_counter != 2)
                {
                    param_error(root->key, root->data.line);
                }
                else if (root->data.arg_arr[0] != T_STRING || root->data.arg_arr[1] != T_INT || (root->data.id_type[0] != T_INT && root->data.id_type[0] != T_UNDEFINED) || (root->data.id_type[1] != T_INT && root->data.id_type[1] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else if (!strcmp(root->key, "chr"))
            {
                if (root->data.param_counter != 1 || root->data.id_counter != 2)
                {
                    param_error(root->key, root->data.line);
                }
                else if (root->data.arg_arr[0] != T_INT || (root->data.id_type[0] != T_STRING && root->data.id_type[0] != T_UNDEFINED) || (root->data.id_type[1] != T_INT && root->data.id_type[1] != T_UNDEFINED))
                {
                    param_error(root->key, root->data.line);
                }
            }
            else //< print
            {
                if (root->data.id_counter != 0)
                {
                    param_error(root->key, root->data.line);
                }
            }

            m.global_table = delete_node(m.global_table, root->key);
        }
    }
}

/**
 * @brief Compares current func id with all possible built_in function
 * @return True if func id is one of built_in function, else false
 */
bool is_built_fun(char *func_id)
{
    if (func_id != NULL)
    {
        for (int i = 0; i < BUILT_FUNC_NUM; i++)
        {
            if (!strcmp(func_id, built_in[i]))
            {
                return true;
            }
        }
    }
    return false;
}

void add_tree()
{
    tree_index++;
    if (tree_index < 100)
        array_of_trees[tree_index] = NULL;
}

bool search_all_trees(char *key)
{
    if (key != NULL)
    {
        for (int i = 0; i <= tree_index; i++)
        {
            if ((node = search_symtable(array_of_trees[i], key)) != NULL)
            {

                return true;
            }
        }
    }
    return false;
}

// search all trees except the current tree
bool search_all_last_trees(char *key)
{
    if(key != NULL)
    {
        for (int i = 0; i < tree_index; i++)
        {
            if ((node = search_symtable(array_of_trees[i], key)) != NULL)
            { 
                return true;
            }
        }
    }
    return false;
}

TData *get_func_data(char *id)
{
    if (id != NULL)
    {
        node = search_symtable(m.global_table, id);
        if (node != NULL)
            return &node->data;
    }
    return NULL;
}

/**
 * @brief Retrieve ID's data from symtable if it exists in current scope
 * @param id Identifier to search for
 * @return Pointer to id's dataset if found, else NULL
 */
TData *get_id_data(char *id)
{
    if (id != NULL)
    {
        for (int idx = tree_index; idx >= 0; idx--)
        {
            node = search_symtable(array_of_trees[idx], id);
            if (node != NULL)
                return &node->data;
        }
    }
    return NULL;
}

TData *get_curscope_id_data(char *id)
{
    if (id != NULL)
    {
        node = search_symtable(array_of_trees[tree_index], id);
        if (node != NULL)
            return &node->data;
    } 
    return NULL;
}

bool define_id_type(char *id, Data_type type, bool in_curr_scope)
{
    if (id != NULL)
    {
        TData *id_data;
        if (in_curr_scope)
            id_data = get_curscope_id_data(id);
        else
            id_data = get_id_data(id);

        if (id_data != NULL)
        {
            id_data->type = type;
            return true;
        }
    }
    return false;
}

void delete_tree() //deletes youngest tree
{
    array_of_trees[tree_index] = NULL;
    tree_index--;
}

/**
 * @brief Syntax of program
 */
void program()
{
    m.global_table = init_symtable(m.global_table);

    GENERATE(gen_header());
    GENERATE(gen_built_func());

    prolog();
    while(true)
    {
        if (!func() && (m.current_token.type == EoF))
        {
            break;
        }
    }

    lasterror_line = -2;
    node = search_symtable(m.global_table, "main");
    if (node == NULL)
    {
        no_definition_error("main", -1);    //< func main was not defined
    }

    if (suspected_tree_idx > -1)    //< check funcs suspected from no_definition
    {
        lasterror_line = 0;
        check_suspected();
        delete_arr_suspected();
    }

    delete_symtable(m.global_table);

    for (int i = 0; i < 100; i++)
    {
        if (array_of_trees[i] != NULL)
        {
            delete_symtable(array_of_trees[i]);
        }
    }

    CHECK_TOKEN(EoF);

    GENERATE(gen_code_end());

    if (error_value == 0) // No error
    {
        flush();
    }
    else
    {
        gen_dispose();
    }
}

/**
 * @brief Checks syntax of function
 */
bool func()
{
    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);

    TData new_data;
    new_data.type = ID;
    new_data.defined = true;
    new_data.is_var = true;
    new_data.is_function = false;
    new_data.in_block = true;
    new_data.is_param = false;

    array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data, "_");
    define_id_type("_", T_UNDEFINED, true);

    bool is_main = false;
    if(!func_header(&is_main))
    {
        return false;
    }

    TData *func_data = get_func_data(m.current_func_id); //< current_func_id can be undefined

    if (func_data != NULL) {
        if (func_data->ret_counter == 0)
            m.current_func_ret_success = true; //< No need to check if function returned correct values
        else
            m.current_func_ret_success = false; //< Check if function returned correct values
    }
    else
    {
        m.current_func_ret_success = true; //< No func id - do not care for return success
    }

    // body
    while(statement());
    if (!m.current_func_ret_success)
        no_return_error(m.current_func_id, m.current_line);

    m.param_counter = 0;    
    var_deep = 0;

    if(is_main)
    {
        GENERATE(gen_main_end());
    }
    else
    {
        if (m.current_func_id != NULL)
            GENERATE(gen_func_end(m.current_func_id));
    }

    delete_symtable(array_of_trees[tree_index]);
    delete_tree();
    return true;
}

/**
 * @brief Checks syntax of function header
 */
bool func_header(bool *is_main)
{
    // checking EOF
    if (!expect_token(KEYWORD,K_FUNC))
    {
        if(!CHECK_NO_ERROR(EoF))
        {
            syntax_error(KEYWORD,m.current_line);
        }
        return false;
    }

    // function id
    bool missing_id = false;
    Token last_func;
    char *func_id;

    if (m.current_func_id != NULL)
    {
        free(m.current_func_id);
    }

    GET_TOKEN();
    if (!CHECK_TOKEN(ID))
    {
        missing_id = true;
        m.current_func_id = NULL;
    }
    else
    {
        func_id = m.current_token.data.s;  //< store func id cause insertion to symtable will be later

        size_t func_id_len = sizeof(char) * (strlen(func_id) + 1);

        m.current_func_id = malloc(func_id_len);
        strncpy(m.current_func_id, func_id, func_id_len);

        bool is_built = is_built_fun(func_id);

        if ((node = search_symtable(m.global_table, func_id)) != NULL || is_built)
        {
            re_definition_error(func_id, m.current_line);    //< redefinition of func is forrbiden
        }

        if (strcmp(func_id, "main") != 0)       //< main's label is created separately
            GENERATE(gen_func_header(func_id));

        new_data_func = init_new_data(new_data_func);
        new_data_func.defined = true;
        new_data_func.is_function = true;
        new_data_func.is_param = false;
        last_func = m.current_token;   //< to make correct free later
    }

    // function parameters
    GET_AND_CHECK(PARENTHESIS_LEFT);
    GET_TOKEN();
    if (!CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        header_arg();
        m.index = 0;
    }

    // function return values
    GET_TOKEN();
    bool inside = false;
    if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
    {
        header_ret();
        inside = true;
        m.index = 0;
    }

    // chcecking {EOL at the end of function header
    if (!inside)
    {
        CHECK_TOKEN(BRACKET_LEFT);
    }
    else if (inside)
    {
        GET_AND_CHECK(BRACKET_LEFT);
    }

    if (!missing_id)
    {
        if (!strcmp(func_id, "main"))
        {
            *is_main = true;
            GENERATE(gen_main_start());

            if (new_data_func.param_counter != 0 || new_data_func.ret_counter != 0)
            {
                main_definition_error(m.current_line);    //< func main cant have param or ret value
            }
        }

        m.global_table = insert_symtable(m.global_table, new_data_func, func_id);    //< insert whole func
        free(last_func.data.s);     //< to free token id

        GENERATE(gen_func_retval(new_data_func.ret_counter));
    }

    GET_AND_CHECK(EOL);
    return true;
}

/**
 * @brief Checks syntax of seqeunce <data type> , <data type> ,...)
 */
void header_ret()
{
    GET_TOKEN();
    if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        return;
    }
    if (IS_DATA_TYPE())
    {
        new_data_func.ret_counter++;
        //add m.actual_token.data.k to local_table for reference in the current block
        switch (m.current_token.data.k)
        {
        case K_INT:
            new_data_func.retval_arr[m.index++] = T_INT;
            break;
        case K_FLOAT64:
            new_data_func.retval_arr[m.index++] = T_FLOAT64;
            break;
        case K_STRING:
            new_data_func.retval_arr[m.index++] = T_STRING;
            break;

        default:
            break;
        }
    }
    GET_TOKEN();

    if (CHECK_NO_ERROR(COMMA))
    {
        header_ret();
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_RIGHT)) // recursion condition
    {
        return;
    }
     else
    {
        syntax_error(m.current_token.type,m.current_line);
    }
}

/**
 * @brief Checks syntax of seqeunce ID <data type> , ID <data type> ,... )
 */
void header_arg()
{
    if (CHECK_TOKEN(ID))
    {
        new_data_var = init_new_data(new_data_var);
        Token prev = m.current_token;

        GET_TOKEN();
        if (IS_DATA_TYPE())
        {

            new_data_var.defined = true;
            new_data_var.is_var = true;
            new_data_var.is_param = true;
            new_data_var.param_num = ++m.param_counter;
            new_data_func.param_counter++;

            switch (m.current_token.data.k)
            {
                case K_INT:
                    new_data_var.type = T_INT;
                    new_data_func.arg_arr[m.index++] = T_INT;
                    array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data_var, prev.data.s);    //< insert parameter
                    break;
                case K_FLOAT64:
                    new_data_var.type = T_FLOAT64;
                    new_data_func.arg_arr[m.index++] = T_FLOAT64;
                    array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data_var, prev.data.s);    //< insert parameter
                    break;
                case K_STRING:
                    new_data_var.type = T_STRING;
                    new_data_func.arg_arr[m.index++] = T_STRING;
                    array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data_var, prev.data.s);    //< insert parameter
                    break;

                default:
                    break;
            }
        }
        free(prev.data.s);      //< free token id
    }

    GET_TOKEN();

    if (CHECK_NO_ERROR(COMMA))
    {
        GET_TOKEN();
        header_arg();
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_RIGHT)) // recursion condition
    {
        return;
    }
    else
    {
        syntax_error(m.current_token.type,m.current_line);
    }
}

/**
 * @brief Checks what statement has come
 * @return False if } was read, else true
 */
bool statement()
{
    // skip EOL's
    GET_TOKEN();
    while(CHECK_NO_ERROR(EOL))
    {
        GET_TOKEN();
    }

    if (CHECK_NO_ERROR(EoF))
    {
        syntax_error(m.current_token.type, m.current_line);
        return false;
    }
    else if (CHECK_NO_ERROR(BRACKET_RIGHT))
    {
        // Block body end
        return false;
    }
    else if (CHECK_NO_ERROR(KEYWORD))
    {
        if (m.current_token.data.k == K_IF)
        {
            if_s();
        }
        else if (m.current_token.data.k == K_FOR)
        {
            if (is_in_for) {
                is_in_nested_for = true;
                GENERATE(gen_start_nested_for());
            }
            for_s();
        }
        else if (m.current_token.data.k == K_RETURN)
        {
            return_s();
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
        }
    }
    else if (CHECK_TOKEN(ID)) //< At this point token must be ID. If not -> syn error
    {
        Token last_id = m.current_token;     //< need to store token's id in case of function call
        TData new_data;
        new_data.type = m.current_token.type;
        new_data.is_var = true;
        new_data.in_block = true;
        new_data.defined = true;
        new_data.is_function = false;
        new_data.is_param = false;

        char *id_name;
        id_name = m.current_token.data.s;

        GET_TOKEN();
        unsigned number_of_id = 1;
        TNode* tmp;
        TData *id_data;
        tmp = NULL;
        id_data = NULL;
        asgn_meta = init_asgn_data(asgn_meta);

        // assignment to var statement with multiple IDs
        if (CHECK_NO_ERROR(COMMA))
        {
            asgn_meta.id_names[number_of_id - 1] = id_name; //< First id on left side of assignment

            id_data = get_id_data(id_name);
            if (id_data != NULL) //< Check if first id is defined
            {
                asgn_meta.id_types[number_of_id - 1] = id_data->type;
                if (id_data->is_param)
                {
                    char buf[3];
                    char *to_gen = malloc(15);
                    strcpy(to_gen,"%param");
                    int2str(id_data->param_num,buf);
                    asgn_meta.id_generate[number_of_id -1] = strcat(to_gen,buf);
                }
                else
                {
                    asgn_meta.id_generate[number_of_id - 1] = id_name;
                }
            }
            else
            {
                asgn_meta.id_types[number_of_id - 1] = T_UNDEFINED;
                no_definition_error(id_name, m.current_line);
            }

            while(!CHECK_NO_ERROR(VAR_ASSIGN))
            {
                if (!CHECK_TOKEN(COMMA))
                {
                    break;
                }
                GET_TOKEN();
                if (!CHECK_TOKEN(ID))
                {
                    break;
                }

                number_of_id++;

                id_name = m.current_token.data.s;
                asgn_meta.id_names[number_of_id - 1] = id_name;
                id_data = get_id_data(id_name);

                if (id_data != NULL)
                {
                    asgn_meta.id_types[number_of_id - 1] = id_data->type;
                    if (id_data->is_param)
                    {
                        char buf[3];
                        char *to_gen = malloc(15);
                        strcpy(to_gen,"%param");
                        int2str(id_data->param_num,buf);
                        asgn_meta.id_generate[number_of_id -1] = strcat(to_gen,buf);
                    }
                    else
                        asgn_meta.id_generate[number_of_id - 1] = id_name;
                }
                else
                {
                    asgn_meta.id_types[number_of_id - 1] = T_UNDEFINED;
                    no_definition_error(id_name, m.current_line);
                }

                // Id name will be freed in assignment_s

                GET_TOKEN();
            }

            assignment_s(number_of_id,false);
            return true; // Don't perform free on last_id (already freed)
        }
        // definition of var statement
        else if (CHECK_NO_ERROR(DEF_OF_VAR))
        {
            if (!strcmp(id_name, "_"))
            {
                other_error(m.current_line);
            }
            tmp = search_symtable(array_of_trees[tree_index], id_name);
            if (tmp == NULL)
            {
                new_data.defined = true;
                new_data.type = T_UNDEFINED;
                array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data, id_name);
            }
            else
            {
                re_definition_error(id_name, m.current_line);
            }

            Data_type expr_type = expression(NO_ASSIGN,false,false);
            if (expr_type != T_UNDEFINED)
                define_id_type(id_name, expr_type, true);

            // Do not free again if the failing expr token was ID or STR (Already free'd)
            if (!CHECK_TOKEN_NOFREE(EOL))
                syntax_error(m.current_token.type,m.current_line);

            bool shadowed = search_all_last_trees(id_name);
            bool for_id_shadowed = search_symtable(m.global_table, id_name);
            if (shadowed || for_id_shadowed)
            { 
                var_deep++;
                if (is_else){       //< shadowed var is in else
                    else_shadow = true;
                }
            }

            if (!is_in_for)
            { 
                GENERATE(gen_var_def(id_name));
            }
            else    //< defvar in for loop will be generated differently
            {
                GENERATE(gen_jump_def(id_name));
                GENERATE(gen_var_in_for(id_name));
            }
            return true;
        }
        // assignment to var statement
        else if (CHECK_NO_ERROR(VAR_ASSIGN))
        {
            asgn_meta.id_names[number_of_id - 1] = id_name;
            id_data = get_id_data(id_name);
            if (id_data != NULL)
            {
                asgn_meta.id_types[number_of_id - 1] = id_data->type;
                if (id_data->is_param)
                {
                    char buf[3];
                    char *to_gen = malloc(15);
                    strcpy(to_gen,"%param");
                    int2str(id_data->param_num,buf);
                    asgn_meta.id_generate[number_of_id -1] = strcat(to_gen,buf);
                }
                else
                {
                    asgn_meta.id_generate[number_of_id - 1] = id_name;
                }
            }
            else
            {
                asgn_meta.id_types[number_of_id - 1] = T_UNDEFINED;
                no_definition_error(id_name, m.current_line);
            }

            assignment_s(number_of_id,false);
            return true; // Don't perform free on last_id (already free'd)
        }
        else if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
        {
            bool is_built = is_built_fun(last_id.data.s);
            number_of_id--;     //< func's id count starts from zero
            function_call(last_id, number_of_id, is_built);
            m.index = 0;

            if (is_built)
            {
                check_built(m.global_table);  //< check semantic of built_in function
            }
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
        }

        free(last_id.data.s);   //< free token's id
    }
    else
    {
        // Invalid statement
        CONSUME_LINE();
    }

    return true;
}

/**
 * @brief Checks syntax of function call
 * @param id Stores the id of calling function
 * @param num_of_id Stores number of ids on the left side of assignment
 * @param is_built Informs whether calling function is built_in or not
 */
void function_call(Token id, unsigned num_of_id, bool is_built)
{
    Token_type previous = PARENTHESIS_LEFT;
    unsigned act_param_counter = 0;
    bool arg_type_error = false;                     //< indicates semantic error
    new_data_func = init_new_data(new_data_func);
    new_data_func.is_function = true;
    new_data_func.line = m.current_line;
    new_data_func.id_counter = num_of_id;
    new_data_func.is_param = false;
    for (unsigned i = 0; i < num_of_id; i++)
        new_data_func.id_type[i] = asgn_meta.id_types[i];

    GENERATE(gen_createframe()); //< create new TF for func params
    while(true)
    {
        GET_TOKEN();
        if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
        {
            if (m.current_func_id != NULL && id.data.s != NULL)
            {
                if ((strcmp(m.current_func_id, "main")) != 0 && !strcmp(id.data.s, "main")) //< main can be called only recursively
                {
                    other_error(m.current_line);
                    break;
                }
                if (!(is_built))
                {
                    TData *id_data;
                    id_data = get_id_data(id.data.s);
                    if (id_data != NULL)  //<var of same name is defined in this scope => function can't be called
                    {
                        re_definition_error2(id.data.s, m.current_line);
                    }

                    if ((node = search_symtable(m.global_table, id.data.s)) != NULL)    //< func is defined for sure
                    {
                        if (node->data.param_counter != act_param_counter)  //< unmatched number of parameters
                        {
                            param_num_error(id.data.s, m.current_line);
                        }

                        if (node->data.ret_counter != num_of_id)    //< unmatched number of ids on left with function return counter
                        {
                            return_unpack_error(id.data.s, m.current_line);
                        }

                        else
                        {
                            for (unsigned idx = 0; idx < num_of_id; idx++)
                            {
                                if (node->data.retval_arr[idx] != asgn_meta.id_types[idx] && asgn_meta.id_types[idx] != T_UNDEFINED)  //< unmatched data type of id on left side with expected return data type
                                {
                                    return_type_error(id.data.s, m.current_line);
                                    break;
                                }
                            }
                        }
                    }
                    else        //< not sure whether func defined
                    {
                        add_suspected_tree();
                        arr_suspected[suspected_tree_idx] = insert_symtable(arr_suspected[suspected_tree_idx], new_data_func, id.data.s);   //< check this func after whole file read
                    }
                }
                else    //< func is built_in, we insert it to global table and check semantics later
                {
                    m.global_table = insert_symtable(m.global_table, new_data_func, id.data.s);
                }

                break;
            }
        }
        else if (CHECK_NO_ERROR(COMMA) && previous != COMMA)
        {
            previous = COMMA;
            continue;
        }

        else if (CHECK_NO_ERROR(INT))
        {
            act_param_counter++;
            if ((node = search_symtable(m.global_table, id.data.s)) != NULL && !(is_built))    //< func is defined for sure
            {
                if (node->data.arg_arr[m.index++] != T_INT)     //< unmatched data type of parameter
                {
                    arg_type_error = true;
                }
            }
            else        //< not sure whether func defined
            {
                new_data_func.param_counter++;
                new_data_func.arg_arr[m.index++] = T_INT;

            }   // end of semantic analysis
            if (strcmp(id.data.s, "print") != 0)
            {
                GENERATE(gen_param_pass(m.current_token, act_param_counter));
            }
            else
            {
                GENERATE(gen_print(m.current_token, act_param_counter));
            }
            previous = INT;
            continue;
        }

        else if (CHECK_NO_ERROR(FLOAT64))
        {
            act_param_counter++;
            if ((node = search_symtable(m.global_table, id.data.s)) != NULL && !(is_built))    //< func is defined for sure
            {
                if (node->data.arg_arr[m.index++] != T_FLOAT64)     ///< unmatched data type of parameter
                {
                    arg_type_error = true;
                }
            }
            else        //< not sure whether func defined
            {
                new_data_func.param_counter++;
                new_data_func.arg_arr[m.index++] = T_FLOAT64;

            }   // end of semantic analysis
            if (strcmp(id.data.s, "print") != 0)
            {
                GENERATE(gen_param_pass(m.current_token, act_param_counter));
            }
            else
            {
                GENERATE(gen_print(m.current_token, act_param_counter));
            }
            previous = FLOAT64;
            continue;
        }

        else if (CHECK_NO_ERROR(STRING))
        {
            act_param_counter++;
            if ((node = search_symtable(m.global_table, id.data.s)) != NULL && !(is_built))    //< func is defined for sure
            {
                if (node->data.arg_arr[m.index++] != T_STRING)     //< unmatched data type of parameter
                {
                    arg_type_error = true;
                }
            }
            else        //< not sure whether func defined
            {
                new_data_func.param_counter++;
                new_data_func.arg_arr[m.index++] = T_STRING;

            }   // end of semantic analysis
            if (strcmp(id.data.s, "print") != 0)
            {
                GENERATE(gen_param_pass(m.current_token, act_param_counter));
            }
            else
            {
                GENERATE(gen_print(m.current_token, act_param_counter));
            }

            free(m.current_token.data.s);
            previous = STRING;
            continue;
        }

        else if (CHECK_NO_ERROR(ID))
        {
            act_param_counter++;
            TData *id_data = get_id_data(m.current_token.data.s);
            if (id_data != NULL)    //< id is defined
            {
                if ((node = search_symtable(m.global_table, id.data.s)) != NULL && !(is_built))    //< func is defined for sure
                {
                    if (node->data.arg_arr[m.index++] != id_data->type) //< unmatched data type of parameter
                    {
                        arg_type_error = true;
                    }
                }
                else    //< not sure whether func defined
                {
                    new_data_func.param_counter++;
                    new_data_func.arg_arr[m.index++] = id_data->type;
                }
            }
            else    //< id is not defined
            {
                new_data_func.param_counter++;
                new_data_func.arg_arr[m.index++] = T_UNDEFINED;
                no_definition_error(m.current_token.data.s, m.current_line);
            }
            if (strcmp(id.data.s, "print") != 0)
            {
                if (id_data != NULL)
                {
                    if (id_data->is_param)
                    {
                        free(m.current_token.data.s);
                        m.current_token.data.s = malloc(1000);
                        char buf[3];
                        int2str(id_data->param_num,buf);
                        strcpy(m.current_token.data.s,"%param");
                        strcat(m.current_token.data.s,buf);
                    }
                    GENERATE(gen_param_pass(m.current_token, act_param_counter));
                    free(m.current_token.data.s);
                }
            }
            else
            {
                if (id_data != NULL)
                {
                    if (id_data->is_param)
                    {
                        free(m.current_token.data.s);
                        m.current_token.data.s = malloc(1000);
                        char buf[3];
                        int2str(id_data->param_num,buf);
                        strcpy(m.current_token.data.s,"%param");
                        strcat(m.current_token.data.s,buf);
                    }
                    GENERATE(gen_print(m.current_token, act_param_counter));
                    free(m.current_token.data.s);
                }
            }

            previous = ID;
            continue;
        }
        else
        {
            syntax_error(m.current_token.type,m.current_line);
            break;
        }
    }
    if (arg_type_error)
    {
        param_type_error(id.data.s, m.current_line);
    }
    if (strcmp(id.data.s, "print") != 0)
    {
        GENERATE(gen_func_call(id.data.s));
    }
}

/**
 * @brief Checks syntax of if statement
 */
void if_s()
{
    int tmp_var_deep = var_deep;
    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);

    // Expression must be of type bool
    GENERATE(if_label());
    expression(NO_ASSIGN,true,false);
    GENERATE(else_jump());

    CHECK_TOKEN_NOFREE(BRACKET_LEFT);
    GET_AND_CHECK(EOL);

    while(statement());
    if (tmp_var_deep != var_deep)
    {
        var_deep--;     //< reduce deep cause entering else block
    }

    // else
    GET_AND_CHECK(KEYWORD);
    if (m.current_token.data.k != K_ELSE)
    {
        syntax_error(m.current_token.type,m.current_line);
    }

    delete_symtable(array_of_trees[tree_index]);
    delete_tree();
    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);
    GENERATE(if_jump()); //skips else branch
    GENERATE(else_label());
    GET_AND_CHECK(BRACKET_LEFT);
    GET_AND_CHECK(EOL);
    is_else = true;

    while(statement());
    GET_AND_CHECK(EOL);
    GENERATE(if_end_label());
    is_else = false;

    if (else_shadow)
    {
        var_deep--;
    }
    
    delete_symtable(array_of_trees[tree_index]);
    delete_tree();
}

/**
 * @brief Checks syntax of assignment to var statement
 * @param number_of_id Number of left side identifiers
 */
void assignment_s(unsigned number_of_id, bool in_for)
{
    bool was_func = false;

    for (unsigned i = 0; i <= number_of_id - 1; i++)
    {
        Data_type expr_type = expression(number_of_id,false,false);
        if (i == number_of_id)
        {
            free(asgn_meta.id_names[i]);
            break;
        }
        if (expr_type == T_FCALL)
        {
            was_func = true;
            for (unsigned j = 0; j < number_of_id; j++)
            {
                bool is__ = false;
                if (strcmp(asgn_meta.id_generate[j],"_") == 0)
                {
                    is__ = true;
                }
                GENERATE(gen_retval_assign(asgn_meta.id_generate[j], j+1,is__));
            }
            GET_TOKEN();
            break;
        }

        if (expr_type != asgn_meta.id_types[i])
        {

            if (strcmp(asgn_meta.id_names[i], "_"))
            {
                compatibility_error(data_types[expr_type], m.current_line);
            }
        }

        if (i != number_of_id-1) // Was the last expr assigned ?
            CHECK_TOKEN_NOFREE(COMMA);
    }
    if (!was_func)
    {
        for (unsigned i = 0; i <= number_of_id - 1; i++)
        {
            bool is__ = false;
            if (strcmp(asgn_meta.id_generate[i],"_") == 0)
            {
                is__ = true;
            }
            GENERATE(gen_var_ass(asgn_meta.id_generate[i],is__));

            if (strcmp(asgn_meta.id_names[i], asgn_meta.id_generate[i]) != 0 && strcmp(asgn_meta.id_generate[i],"") != 0)
                free(asgn_meta.id_generate[i]);

            free(asgn_meta.id_names[i]);
        }
    }
    if (!in_for)
        CHECK_TOKEN_NOFREE(EOL);
}

/**
 * @brief Checks syntax of for statement
 */
void for_s()
{
    is_in_for = true;
    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);

    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
        char *id_name;
        id_name = m.current_token.data.s;
        TData new_data;
        new_data.type = T_UNDEFINED;
        new_data.is_var = true;
        new_data.in_block = true;
        new_data.defined = true;
        new_data.is_function = false;
        new_data.is_param = false;

        GET_AND_CHECK(DEF_OF_VAR);

        array_of_trees[tree_index] = insert_symtable(array_of_trees[tree_index], new_data, id_name);
        m.global_table = insert_symtable(m.global_table, new_data, id_name);    //< we need to have it acessable also when for is not actual

        Data_type expr_type = expression(NO_ASSIGN,false,false);
        if (expr_type != T_UNDEFINED)
            define_id_type(id_name, expr_type, true);

        CHECK_TOKEN_NOFREE(SEMICLN);

        bool is_shadowed = search_all_last_trees(id_name);
        if(is_shadowed)
            var_deep++;

        if (!is_in_nested_for) {
            GENERATE(gen_var_def(id_name));
        }
        else
        {
            GENERATE(gen_jump_def(id_name));
            GENERATE(gen_var_in_for(id_name));
        }
        
        free(id_name);
    }
    else if (CHECK_TOKEN(SEMICLN)){;}

    GENERATE(for_header());

    // condition
    expression(NO_ASSIGN,true,false);
    CHECK_TOKEN_NOFREE(SEMICLN);

    GENERATE(for_condition_eval());

    // increment / decrement (can be epmty)
    GET_TOKEN();
    if(CHECK_NO_ERROR(ID))
    {
        // same code as in assignment_s
        char *id_name;
        id_name = m.current_token.data.s;

        GET_TOKEN();
        unsigned number_of_id = 1;

        TData *id_data;
        id_data = NULL;
        asgn_meta = init_asgn_data(asgn_meta);

        // assignment to var statement with multiple IDs
        if (CHECK_NO_ERROR(COMMA))
        {
            asgn_meta.id_names[number_of_id - 1] = id_name; //< First id on left side of assignment

            id_data = get_id_data(id_name);
            if (id_data != NULL) //< Check if first id is defined
            {
                asgn_meta.id_types[number_of_id - 1] = id_data->type;
                if (id_data->is_param)
                {
                    char buf[3];
                    char *to_gen = malloc(15);
                    strcpy(to_gen,"%param");
                    int2str(id_data->param_num,buf);
                    asgn_meta.id_generate[number_of_id -1] = strcat(to_gen,buf);
                }
                else
                    asgn_meta.id_generate[number_of_id - 1] = id_name;
            }
            else
            {
                asgn_meta.id_types[number_of_id - 1] = T_UNDEFINED;
                no_definition_error(id_name, m.current_line);
            }

            while(!CHECK_NO_ERROR(VAR_ASSIGN))
            {
                if (!CHECK_TOKEN(COMMA))
                {
                    break;
                }
                GET_TOKEN();
                if (!CHECK_TOKEN(ID))
                {
                    break;
                }

                number_of_id++;

                id_name = m.current_token.data.s;
                asgn_meta.id_names[number_of_id - 1] = id_name;
                id_data = get_id_data(id_name);

                if (id_data != NULL)
                {
                    asgn_meta.id_types[number_of_id - 1] = id_data->type;
                    if (id_data->is_param)
                    {
                        char buf[3];
                        char *to_gen = malloc(15);
                        strcpy(to_gen,"%param");
                        int2str(id_data->param_num,buf);
                        asgn_meta.id_generate[number_of_id -1] = strcat(to_gen,buf);                    
                    }
                    else
                        asgn_meta.id_generate[number_of_id - 1] = id_name;
                }
                else
                {
                    asgn_meta.id_types[number_of_id - 1] = T_UNDEFINED;
                    no_definition_error(id_name, m.current_line);
                }

                GET_TOKEN();
            }
            assignment_s(number_of_id, true);
        }
        else if (CHECK_NO_ERROR(VAR_ASSIGN))
        {
            asgn_meta.id_names[number_of_id - 1] = id_name;
            id_data = get_id_data(id_name);
            if (id_data != NULL)
            {
                asgn_meta.id_types[number_of_id - 1] = id_data->type;
                if (id_data->is_param)
                {
                    char buf[3];
                    char *to_gen = malloc(15);
                    strcpy(to_gen,"%param");
                    int2str(id_data->param_num,buf);
                    asgn_meta.id_generate[number_of_id -1] = strcat(to_gen,buf);
                }
                else
                    asgn_meta.id_generate[number_of_id - 1] = id_name;
            }
            else
            {
                asgn_meta.id_types[number_of_id - 1] = T_UNDEFINED;
                no_definition_error(id_name, m.current_line);
            }

            assignment_s(number_of_id,true);
        }

        CHECK_TOKEN_NOFREE(BRACKET_LEFT);
    }
    else if (CHECK_TOKEN(BRACKET_LEFT)){;}

    add_tree();
    array_of_trees[tree_index] = init_symtable(array_of_trees[tree_index]);

    GET_AND_CHECK(EOL);
    GENERATE(for_body());
    // body
    while(statement());
    GENERATE(for_end());
    delete_symtable(array_of_trees[tree_index]);
    delete_tree();
    delete_symtable(array_of_trees[tree_index]);
    delete_tree();
    if (is_in_nested_for)
    {
        is_in_nested_for = false;
        is_in_for = true;
        GENERATE(gen_end_nested_for());
    }
    else
    {
        is_in_for = false;
        GENERATE(gen_clear_iter());
    }   
}

/**
 * @brief Checks syntax of return statement
 */
void return_s()
{
    TData *func_data = get_func_data(m.current_func_id);

    if (func_data == NULL)
    {
        // Consume the rest of the line (cannot determine how many expressions are required)
        CONSUME_LINE();
    }
    else
    {
        if (func_data->ret_counter == 0) //< return in void function
        {
            bool ret_multiple = false; //< Check for syntax error if void func has 2+ retvals
            while (true)
            {
                Data_type expr_type = expression(NO_ASSIGN,false,true);

                // Accepts only commas and linefeeds
                if (CHECK_NO_ERROR(COMMA))
                {
                    if (!ret_multiple)
                    {
                        if (expr_type == T_UNDEFINED)
                        {
                            // First retval is of type UNDEFINED - no trailing commas allowed
                            if (!CHECK_NO_ERROR(COMMA))
                            {
                                ret_multiple = true;
                                continue;
                            }
                            // ELSE - void return followed by comma
                            // Continue to syntax error
                        }
                        else
                        {
                            // First retval has a valid type - allow gathering more retvals
                            ret_multiple = true;
                            continue;
                        }
                    }
                    else
                    {
                        if (expr_type != T_UNDEFINED)
                        {
                            continue;
                        }
                        // ELSE - return from a void function with undefined n-th retval
                        // Continue to syntax error
                    }
                }
                else if (CHECK_NO_ERROR(EOL))
                {
                    if (ret_multiple)
                    {
                        if (expr_type != T_UNDEFINED)
                        {
                            // SUCCESSful return
                            if (!m.current_func_ret_success)
                                m.current_func_ret_success = true;
                            break;
                        }
                        // ELSE - last expression (that is not first retval)
                        //        vas undefined - cannot assign T_UNDEFINED
                        // Continue to syntax error
                    }
                    else
                    {
                        // SUCCESSful return
                        if (!m.current_func_ret_success)
                            m.current_func_ret_success = true;
                        break;
                    }
                }

                // Missing return value or EOF
                syntax_error(m.current_token.type,m.current_line);
                break;
            }
        }
        else
        {
            unsigned expr_idx = 0;
            bool ret_type_error = false; //< Delay type errors so syntax is checked first

            while (true)
            {
                Data_type expr_type = expression(NO_ASSIGN,false,false);

                // If expr_type is undefined, error was already thrown or an empty expr was parsed
                if (expr_type != T_UNDEFINED && expr_idx < func_data->ret_counter) {
                    if (expr_type != func_data->retval_arr[expr_idx])
                        // Returning wrong data type
                        ret_type_error = true;
                }

                if (CHECK_NO_ERROR(COMMA))
                {
                    // If expr is undefined, error was thrown or no expression was parsed (empty return)
                    if (expr_type != T_UNDEFINED)
                    {
                        expr_idx++;
                        continue;
                    }
                    // Continue to syntax error
                }
                else if (CHECK_NO_ERROR(EOL))
                {
                    // Expr type is undefined on the end of the return statement
                    // if no error was thrown yet, expr is missng = syntax error
                    if (expr_type != T_UNDEFINED)
                    {
                        if (expr_idx != func_data->ret_counter - 1)
                        {
                            // Insufiicient or too many return values (expressions)
                            return_num_error(m.current_func_id, m.current_line);
                        }
                        else
                        {
                            // SUCCESSful return
                            if (!m.current_func_ret_success)
                                m.current_func_ret_success = true;
                        }
                        break;
                    }
                    // Continue to syntax error
                }

                // Missing return value or EOF
                syntax_error(m.current_token.type,m.current_line);
                break;
            }

            if (ret_type_error)
                return_type_error(m.current_func_id, m.current_line);
        }
        GENERATE(gen_func_return(m.current_func_id, func_data->ret_counter));
    }
}

/**
 * @brief Parses a single expression
 */
Data_type expression(unsigned num_of_id, bool is_bool, bool can_be_empty)
{
    bool func_call = false;
    Data_type expr_type = T_UNDEFINED;

    if (!expr(&expr_type, &func_call, num_of_id, is_bool, can_be_empty))
    {
        syntax_error(m.current_token.type,m.current_line);

        CONSUME_FAILED_EXPR();

        return T_UNDEFINED; //< Invalid expression's data type
    }

    if (func_call)
    {
        return T_FCALL;
    }

    else
        return expr_type;
}

/**
 * @brief Reads input for expressions and ajusts it
 * @param input_terminal Next token's relevant data - passed by reference to be collected
 * @param func_call Identifier for function calls - is set to true if input is function call
 * @return true on successful read & data collection, else false
 */
bool expr_input(Terminal *input_terminal, bool *func_call, unsigned num_of_id, unsigned *param_num)
{
    m.previous_token = m.current_token;
    GET_TOKEN();

    input_terminal->current_line = m.current_line;
    input_terminal->token.type = m.current_token.type;
    input_terminal->token.data.s = NULL;

    if (CHECK_NO_ERROR(ADD))
    {
        input_terminal->terType = ADDS;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(SUB))
    {
        input_terminal->terType = SUBS;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(MUL))
    {
        input_terminal->terType = MULS;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(DIV))
    {
        input_terminal->terType = DIVS;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_LEFT))
    {
        if (m.previous_token.type == ID)
        {
            bool is_built = is_built_fun(m.previous_token.data.s);
            function_call(m.previous_token,num_of_id,is_built);
            *func_call = true;

            m.index = 0;
            if (is_built)
            {
                check_built(m.global_table);  //< check semantic of built_in function
            }
        }
        input_terminal->terType = LP;
        // Function return type from symtable
    }
    else if (CHECK_NO_ERROR(PARENTHESIS_RIGHT))
    {
        input_terminal->terType = RP;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(ID) || CHECK_NO_ERROR(INT) ||
             CHECK_NO_ERROR(STRING) || CHECK_NO_ERROR(FLOAT64))
    {
        input_terminal->terType = II;

        if (CHECK_NO_ERROR(ID) || CHECK_NO_ERROR(STRING))
        {
            input_terminal->token.data.s = m.current_token.data.s;

            if (CHECK_NO_ERROR(ID))
            {
                m.previous_token = m.current_token;

                // ID data type from symtable
                TData *id_data = get_id_data(m.current_token.data.s);
                if (id_data != NULL)
                    input_terminal->dataType = id_data->type;
                else
                    input_terminal->dataType = T_UNDEFINED;

                if (id_data != NULL)
                    if (id_data->is_param)
                        *param_num = id_data->param_num;
            }
            else
            {
                input_terminal->dataType = T_STRING;
            }
            //free(m.current_token.data.s);
        }
        else
        {
            if (CHECK_NO_ERROR(INT))
                input_terminal->token.data.i = m.current_token.data.i;
            else
                input_terminal->token.data.d = m.current_token.data.d;

            // Hack - Data_type and Token_type have the same values for INT, STR & FLOAT
            input_terminal->dataType = m.current_token.type;
        }
    }
    else if (CHECK_NO_ERROR(GT))
    {
        input_terminal->terType = GTS;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(LT))
    {
        input_terminal->terType = LTS;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(EQ))
    {
        input_terminal->terType = EQS;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(NE))
    {
        input_terminal->terType = NES;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(LE))
    {
        input_terminal->terType = LES;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(GE))
    {
        input_terminal->terType = GES;
        input_terminal->dataType = T_NIL;
    }
    else if (CHECK_NO_ERROR(EOL) || CHECK_NO_ERROR(COMMA) ||
             CHECK_NO_ERROR(SEMICLN) || CHECK_NO_ERROR(BRACKET_LEFT))
    {
        input_terminal->terType = EN;
        input_terminal->dataType = T_NIL;
    }
    else
    {
        return false;
    }

    return true;
}

/**
 * @brief Checks syntax of the prolog
 */
void prolog()
{
    if (!expect_token(KEYWORD, K_PACKAGE))
    {
        syntax_error(KEYWORD,m.current_line);
    }

    if (expect_token(ID,K_ERROR))
    {
        if (strcmp("main",m.current_token.data.s) != 0)
        {
            syntax_error(m.current_token.type,m.current_line);
        }
        free(m.current_token.data.s);
    }
    else
    {
        syntax_error(m.current_token.type,m.current_line);
    }
}

/**
 * @brief Skip EOL's and expect certain token to come
 * @param t_type is token type to expect
 * @param k is expected Keyword, if expecting other than Keyword, set to K_ERROR
 * @return true if token came, else false
 */
bool expect_token(Token_type t_type, Keyword k)
{
    while(true)
    {
        GET_TOKEN();
        if (CHECK_NO_ERROR(t_type) && ((k == K_ERROR)? true : m.current_token.data.k == k))
        {
            return true;
        }
        else if (m.current_token.type == EOL)
        {
            continue;
        }
        else
        {
            return false;
        }
    }
}
