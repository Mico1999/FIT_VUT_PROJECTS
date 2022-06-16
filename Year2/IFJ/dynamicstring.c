/**
 * @file dynamicstring.c
 * @authors Rebeka Cernianska, Peter Rucek, Matej Jurik
 * @date 16 Oct 2020
 * @brief Flexible buffer for saving token data
 */

#include "dynamicstring.h"

/**
 * @brief Initialize dynstr before use
 * @param str Valid pointer to declared dynstr to be initalized
 */
void dynstr_init(dynamic_string *str)
{
    str->len = 0;
    str->alloc_len = 0;
    str->buff = NULL;
}

/**
 * @brief Append character to dynstr's buffer
 * @param str Valid pointer to initialized dynstr
 * @param c Char to be appended
 * @return 1 if append was successfull,
 *         0 if allocation error occured
 */
int add_char(dynamic_string *str, char c)
{
    if (str->len == 0) //< Allocate memory if buffer is empty
    {
        str->buff = malloc(BLOCK);

        if (str->buff == NULL)
        {
            intern_error();
            return 0;
        }

        str->alloc_len = BLOCK;
    }
    else if (str->len+1 >= str->alloc_len) //< Reallocate memory when more is needed
    {
        str->buff = realloc(str->buff, str->alloc_len+BLOCK);

        if (str->buff == NULL)
        {
            intern_error();
            return 0;
        }

        str->alloc_len += BLOCK;
    }

    str->buff[str->len++] = c; //< Append character
    str->buff[str->len] = '\0'; //< Nullchar-terminate the buffer
    return 1;
}


/**
 * @brief Append a string to dynstr's buffer
 * @param str Valid pointer to initialized dynstr
 * @param str_to_copy String to be appended
 * @return 1 if append was successfull,
 *         0 if allocation error occured
 */
int add_string(dynamic_string *str, char *str_to_copy)
{
    if (str->len == 0) //< If the buffer is empty, allocate memory
    {
        str->buff = malloc(strlen(str_to_copy)+1); //< Only allocate the needed space

        if (str->buff == NULL)
        {
            intern_error();
            return 0;
        }

        str->alloc_len = strlen(str_to_copy); //< Update how much space is available in the buffer currently
    }
    else if (str->len+strlen(str_to_copy) >= (unsigned)str->alloc_len) //< Not enough space ?
    {
        str->buff = realloc(str->buff, str->alloc_len+strlen(str_to_copy)+1); //< Only realloc the amount of space needed

        if (str->buff == NULL)
        {
            intern_error();
            return 0;
        }

        str->alloc_len += strlen(str_to_copy);
    }

    for (unsigned int i = 0; i < strlen(str_to_copy); i++) //< Transfer data to the target buffer
    {
        str->buff[i+str->len] = str_to_copy[i];
    }

    str->len = strlen(str_to_copy)+str->len;
    str->buff[str->len] = '\0';

    return 1;
}


/**
 * @brief Append a variable number of linefeed terminated strings to dynstr's buffer
 * @param str Valid pointer to initialized dynstr
 * @param ... List of strings to be appended
 * @return 1 if append was successfull,
 *         0 if allocation error occured
 */
int add_strings(dynamic_string *str, ...)
{
    va_list args;

    va_start(args, str); //< Variadic args come after str param

    char *str_to_copy = va_arg(args, char*); //< Get next va_arg

    // args' size can not be determined - use linefeed termination
    while(strcmp(str_to_copy, "\n") != 0) 
    {
        if (!add_string(str, str_to_copy)) return 0;

        str_to_copy = va_arg(args, char*); 
    }

    if (!add_char(str, '\n')) return 0; 

    va_end(args);
    return 1;
}

/**
 * @brief Destruct dynstr - Free used memory and reset other members to default values.
 *        Allows to re-use buffer, needs to be used before a buffer is used again.
 * @param str Valid pointer to initialized dynstr
 */
void dynstr_free(dynamic_string *str)
{
    if (str->buff != NULL)
    {
        free(str->buff);
    }
    str->len = 0;
    str->alloc_len = 0;
}

/**
 * @brief Compare dyn_str's buffer to passed string
 * @param str Valid pointer to initialized dynstr
 * @param const_str String to compare the dyn_str's buffer against
 * @return 1 on match,
 *         0 on mismatch
 */
int dynstr_cmp(dynamic_string *str, const char *const_str)
{
    if (str->buff == NULL || const_str == NULL)
    {
        return 0;
    }
    // strcmp returns 0 on match, however, return 1 on match for convenience
    return (strcmp(str->buff, const_str) == 0) ? 1 : 0;
}

void copy_token_string(char *token_string, dynamic_string *string)
{
    for (int i = 0; i < string->len; i++)
    {
        token_string[i] = string->buff[i];
    }
    token_string[string->len] = '\0';
}
