#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

struct element{

    int data;
    struct element *next;
};

typedef struct Stack{

    struct element *head;
} Stack;

Stack create_stack(); //returns an empty stack

char push(Stack *, int); //return 0 - could not allocate memory, 1 - pushed successfully

char check_if_empty(Stack); //1 - empty, 0 - not empty

char check_if_full(Stack *); //1 - could not allocate more memory, 0 - memory allocated

char pop(Stack *, int *); //return 0 - stack empty, 1 - popped successfully

void free_stack(Stack *);

#endif // STACK_H_INCLUDED
