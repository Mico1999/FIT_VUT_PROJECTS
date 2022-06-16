/**
 * @file stack.h
 * @authors Peter Rucek
 * @date 16 Nov 2020
 * @brief Interface for stack
 */

#ifndef _STACK_H_
#define _STACK_H_

#include "lexer.h"

#define STACK_SIZE 100
#define STACK_ERROR -312

typedef struct stack { 
    int top; 
    int capacity; 
    int* array; 
} Stack ; 

Stack* createStack(int capacity);
int isFull(Stack* stack);
int isEmpty(Stack* stack);
void push(Stack* stack, int item);
int pop(Stack* stack);
int top(Stack* stack);
void deleteStack(Stack *stack);

#endif