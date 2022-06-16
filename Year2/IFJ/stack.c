/**
 * @file stack.c
 * @authors Peter Rucek
 * @date 16 Nov 2020
 * @brief Imlemenatation of stack
 */

#include "stack.h"


// function to create a stack of given capacity. It initializes size of 
// stack as 0 
Stack* createStack(int capacity) 
{ 
    Stack* stack = malloc(sizeof(struct stack)); 
    if (stack == NULL)
    {
        return NULL;
    }
    stack->capacity = capacity; 
    stack->top = -1; 
    stack->array = (int*)malloc(stack->capacity * sizeof(int)); 
    if (stack->array == NULL)
    {
        return NULL;
    }
    for (int i = 0; i < STACK_SIZE; i++)
        stack->array[i] = -1;
    return stack; 
} 
  
// Stack is full when top is equal to the last index 
int isFull(Stack* stack) 
{ 
    return stack->top == stack->capacity - 1; 
} 
  
// Stack is empty when top is equal to -1 
int isEmpty(Stack* stack) 
{ 
    return stack->top == -1; 
} 
  
// Function to add an item to stack.  It increases top by 1 
void push(Stack* stack, int item) 
{ 
    if (isFull(stack)) 
        return; 
    stack->array[++stack->top] = item; 
} 
  
// Function to remove an item from stack.  It decreases top by 1 
int pop(Stack* stack) 
{ 
    if (isEmpty(stack)) 
        return STACK_ERROR; 
    return stack->array[stack->top--]; 
} 
  
// Function to return the top from stack without removing it 
int top(Stack* stack) 
{ 
    if (isEmpty(stack)) 
        return STACK_ERROR; 
    return stack->array[stack->top]; 
} 

void deleteStack(Stack *stack)
{
    free(stack->array);
    free(stack);
}
