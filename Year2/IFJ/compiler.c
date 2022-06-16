/**
 * @file compiler.c
 * @authors Rebeka Cernianska, Marek Micek, Matej Jurik, Peter Rucek
 * @date 25 Oct 2020
 * @brief Parser Tests
 */

#include "parser.h"

int main()
{
    program();
    fprintf(stderr,"Error code: %d\n",error_value);
    return error_value;
}