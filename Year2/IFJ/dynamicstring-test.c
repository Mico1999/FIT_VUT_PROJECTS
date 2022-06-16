/**
 * @file dynamicstring-test.c
 * @authors Rebeka Cernianska, Peter Rucek ...
 * @date 16 Oct 2020
 * @brief Dynamic string tests
 */

#include "dynamicstring.h"

int main()
{
    dynamic_string str;
    dynstr_init(&str);
    char *tocopy = "int";
    const char *copy = "func";

    add_string(&str, tocopy);

    printf("%d\n", dynstr_cmp(&str, copy));

    for (int i = 0; i < 10; i++)
    {
        add_string(&str, tocopy);
    }

    printf("pocet alokovanych miest:%d, pocet znakov:%d\n", str.alloc_len, str.len);
    printf("%s\n",str.buff);

    dynstr_free(&str);

    dynstr_init(&str);
    add_string(&str, tocopy);

    add_char(&str, ' ');
    add_char(&str, 'i');
    add_char(&str, 't');
    add_char(&str, '\'');
    add_char(&str, 's');
    add_char(&str, ' ');
    add_char(&str, 'm');
    add_char(&str, 'e');
    add_char(&str, '!');

    printf("pocet alokovanych miest:%d, pocet znakov:%d\n", str.alloc_len, str.len);
    printf("%s\n", str.buff);
    
    dynstr_free(&str);


}