#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack init()
{
    Stack s;
    s.head = NULL;
    return s;
}

int ssize(struct element *stack_head)
{
    if (stack_head != NULL)
        return 1 + ssize(stack_head->next);

    return 0;
}

char push(Stack *s, stype data)
{
    struct element *node = (struct element *)malloc(sizeof(struct element));

    if (node == NULL)
        return 0;
    else
    {
        node->data = data;
        node->next = (*s).head;
        s->head = node;

        return 1;
    }
}

char is_full(Stack *s)
{
    struct element *node = (struct element *)malloc(sizeof(struct element));
    free(node);
    return node == NULL;
}

char is_empty(Stack s)
{
    return s.head == NULL;
}

char pop(Stack *s, stype *x)
{

    if ((*s).head != NULL)
    {
        struct element *temp_ptr = (*s).head;
        stype value = ((*s).head)->data;
        (*s).head = ((*s).head)->next;
        free(temp_ptr);
        *x = value;
        return 1;
    }
    else
    {
        return 0;
    }
}

void free_stack(Stack *s)
{
    struct element *temp_ptr;

    while (((*s).head) != NULL)
    {
        temp_ptr = (*s).head;
        (*s).head = ((*s).head)->next;
        free(temp_ptr);
    }
}
