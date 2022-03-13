#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stack.h"
#include "calc.h"
int isnumber(const char *string);
void bin_op_stack(Stack *stack, long (*f)(long, long));
char *clean_token(char *s);
long fadd(long a, long b);
long fminus(long a, long b);
long ftimes(long a, long b);
char handle_token(Stack *stack, char *dirty_token);

#define BUFSIZE 1024

void (*global_callback)(const char const *message) = NULL;

int calculate(char *input, long *result, void (*message_callback)(const char const *message))
{
    global_callback = message_callback;
    Stack stack = init();
    char *token = strtok(input, " ");

    while (token != NULL)
    {
        if (!handle_token(&stack, token))
            return 0;
        token = strtok(NULL, " ");
    }

    const char result_status = pop(&stack, result);

    free_stack(&stack);
    global_callback = NULL;
    return result_status;
}

int isnumber(const char *string)
{
    int i = 0;
    const int length = strlen(string);
    const char is_probably_negative = length > 1 && string[0] == '-';
    if (is_probably_negative)
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

long fminus(long a, long b)
{
    if (global_callback != NULL)
    {
        char message[BUFSIZE];
        sprintf(message, "%ld - %ld\n", a, b);
        global_callback(message);
    }
    return a - b;
}

long ftimes(long a, long b)
{
    if (global_callback != NULL)
    {
        char message[BUFSIZE];
        sprintf(message, "%ld * %ld\n", a, b);
        global_callback(message);
    }
    return a * b;
}

char *clean_token(char *s)
{
    const int n = strlen(s);
    char *accumulator = (char *)calloc(n, sizeof(char));

    int j = 0;
    for (int i = j = 0; i < n; i++)
        if (isalnum(s[i]) || s[i] == '-' || s[i] == '+' || s[i] == '*')
            accumulator[j++] = s[i];

    accumulator[j] = '\0';

    return accumulator;
}

char handle_token(Stack *stack, char *dirty_token)
{
    const char *token = clean_token(dirty_token);
    const int token_length = strlen(token);

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
        free((void *)token);
        return 0;
    }

    printf("[%s]\n", token);
    free((void *)token);
    return 1;
}