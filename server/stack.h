#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

typedef long stype;

struct element
{
    stype data;
    struct element *next;
};

typedef struct Stack
{
    struct element *head;
} Stack;

Stack init(); // returns an empty stack

int ssize(struct element *);

char push(Stack *, stype); // return 0 - could not allocate memory, 1 - pushed successfully

char is_empty(Stack); // 1 - empty, 0 - not empty

char is_full(Stack *); // 1 - could not allocate more memory, 0 - memory allocated

char pop(Stack *, stype *); // return 0 - stack empty, 1 - popped successfully

void free_stack(Stack *);

#endif // STACK_H_INCLUDED
