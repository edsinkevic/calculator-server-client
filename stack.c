#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

Stack create_stack()
{
    Stack s;
    s.head = NULL;
    return s;
}

char push(Stack *s, int data)
{
    struct element *node = (struct element *)malloc(sizeof(struct element));

    if(node == NULL)
        return 0;
    else
    {
        node -> data = data;
        node -> next = (*s).head;
        (*s).head = node;

        return 1;
    }
}

char check_if_full(Stack *s)
{
    struct element *node = (struct element *)malloc(sizeof(struct element));
    free(node);
    return node == NULL;
}

char check_if_empty(Stack s)
{
    return s.head == NULL;
}

char pop(Stack *s, int *x)
{

    if((*s).head != NULL)
    {
        struct element *temp_ptr = (*s).head;
        int value = ((*s).head) -> data;
        (*s).head = ((*s).head) -> next;
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

    while(((*s).head) != NULL)
    {
        temp_ptr = (*s).head;
        (*s).head = ((*s).head) -> next;
        free(temp_ptr);
    }
}
