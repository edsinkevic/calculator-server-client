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

Stack sinit(); // returns an empty stack

int ssize(Stack);

char spush(Stack *, stype); // return 0 - could not allocate memory, 1 - pushed successfully

char sempty(Stack); // 1 - empty, 0 - not empty

char sfull(Stack *); // 1 - could not allocate more memory, 0 - memory allocated

char spop(Stack *, stype *); // return 0 - stack empty, 1 - popped successfully

void sfree(Stack *);

void sprint(Stack);

#endif // STACK_H_INCLUDED
