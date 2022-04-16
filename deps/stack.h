#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <stdint.h>

typedef int int32_t;

#ifndef STYPE
#define STYPE int32_t
#endif

typedef struct _stack stack;

stack *sinit();  // returns an empty stack

int ssize(stack *);

char spush(stack *, STYPE);  // return 0 - could not allocate memory, 1 - pushed successfully

char sempty(stack *);  // 1 - empty, 0 - not empty

char sfull(stack *);  // 1 - could not allocate more memory, 0 - memory allocated

char spop(stack *, STYPE *);  // return 0 - stack empty, 1 - popped successfully

void sfree(stack *);

void sprint(stack *);

#endif  // STACK_H_INCLUDED
