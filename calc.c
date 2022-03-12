#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "calc.h"
int isnumber(char *string);
void bin_op_stack(Stack *stack, long (*f)(long, long));
char *clean_token(char *s);
long fadd(long a, long b);
long fminus(long a, long b);
long ftimes(long a, long b);
char handle_token(Stack *stack, char *dirty_token);

#define BUFSIZE 1024

void (*global_callback)(char *message) = NULL;

int calculate(char *input, long *result, void (*message_callback)(char *message))
{
    global_callback = message_callback;
    char result_status;
    Stack stack = init();
    char *token = strtok(input, " ");

    while (token != NULL)
    {
        if (!handle_token(&stack, token))
            return 0;
        token = strtok(NULL, " ");
    }

    result_status = pop(&stack, result);

    free_stack(&stack);
    return result_status;
}

int isnumber(char *string)
{
    int i = 0;
    int length = strlen(string);
    if (length > 1 && string[0] == '-')
        ++i;

    for (i; i < length; ++i)
        if (!isdigit(string[i]))
            return 0;

    return 1;
}

void bin_op_stack(Stack *stack, long (*f)(long, long))
{
    if (ssize(stack->head) >= 2)
    {
        long popped_value1;
        pop(stack, &popped_value1);
        long popped_value2;
        pop(stack, &popped_value2);
        push(stack, (*f)(popped_value1, popped_value2));
    }
}

long fadd(long a, long b)
{
    if (global_callback != NULL)
    {
        char message[BUFSIZE];
        sprintf(message, "%ld + %ld\n", a, b);
        global_callback(message);
    }

    return a + b;
}
long fminus(long a, long b) { return a - b; }
long ftimes(long a, long b) { return a * b; }

char *clean_token(char *s)
{
    int j;
    int n = strlen(s);
    char *accumulator = (char *)calloc(n, sizeof(char));

    for (int i = j = 0; i < n; i++)
        if (isalnum(s[i]) || s[i] == '-' || s[i] == '+' || s[i] == '*')
            accumulator[j++] = s[i];

    accumulator[j] = '\0';

    return accumulator;
}

char handle_token(Stack *stack, char *dirty_token)
{
    char *token = clean_token(dirty_token);
    int token_length = strlen(token);

    printf("length: %d\n", token_length);

    if (isnumber(token) == 1)
        push(stack, atol(token));
    else if (token_length == 1)
    {
        if (token[0] == '+')
            bin_op_stack(stack, &fadd);
        else if (token[0] == '-')
            bin_op_stack(stack, &fminus);
        else if (token[0] == '*')
            bin_op_stack(stack, &ftimes);
    }
    else
    {
        free(token);
        return 0;
    }

    printf("[%s]\n", token);

    free(token);
    return 1;
}