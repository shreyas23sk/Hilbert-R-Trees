#ifndef STACK_H
#define STACK_H
#include "bool.h"
#include "structs.h"

typedef struct Stack Stack; // Stack is a pointer to a struct stack
// Returns a pointer to a new stack. Returns NULL if memory allocation
Stack *newStack();
// Pushes element onto stack. Returns false if memory allocation fails
bool push(Stack *stack, NODE element);
// Returns a pointer to the top element. Returns NULL if stack is empty
NODE top(Stack *stack);
// Pops the top element and returns true. Returns false if stack is empty
bool pop(Stack *stack);

bool isEmpty(Stack *stack);
// Frees all memory associated with stack
void freeStack(Stack *stack);
#endif