#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "calc.h"
#include "stack.h"

int isnumber(const char *string);
char bin_op_stack(stack *stack, long (*f)(long, long));
long fadd(long a, long b);
long fminus(long a, long b);
long ftimes(long a, long b);
char handle_token(stack *stack, char *dirty_token);
char cpred(char c);

#define BUFSIZE 1024

void (*EXT_CALLBACK)(const char const *message) = NULL;

int calculate(char *unprocessed_input, long *result, void (*MSG_CALLBACK)(const char const *message))
{
    EXT_CALLBACK = MSG_CALLBACK;

    char input[BUFSIZE];
    memset(input, 0, BUFSIZE);
    strncpy(input, unprocessed_input, BUFSIZE);

    stack *st = sinit();
    char *t = strtok(input, " ");

    while (t != NULL)
    {
        if (!handle_token(st, t))
        {
            sfree(&st);
            return 0;
        }
        t = strtok(NULL, " ");
    }

    const char result_status = spop(st, result);

    sprint(st);
    sfree(st);
    EXT_CALLBACK = NULL;
    return result_status;
}

char bin_op_stack(stack *st, long (*f)(long, long))
{
    char status = 0;
    if (ssize(st) >= 2)
    {
        long b, a;
        status = spop(st, &b);
        status = spop(st, &a);
        status = spush(st, (*f)(a, b));
    }

    return status;
}

long fadd(long a, long b)
{
    if (EXT_CALLBACK != NULL)
    {
        char message[BUFSIZE];
        sprintf(message, "%ld + %ld\n", a, b);
        EXT_CALLBACK(message);
    }
    return a + b;
}

long fminus(long a, long b)
{
    if (EXT_CALLBACK != NULL)
    {
        char message[BUFSIZE];
        sprintf(message, "%ld - %ld\n", a, b);
        EXT_CALLBACK(message);
    }
    return a - b;
}

long ftimes(long a, long b)
{
    if (EXT_CALLBACK != NULL)
    {
        char message[BUFSIZE];
        sprintf(message, "%ld * %ld\n", a, b);
        EXT_CALLBACK(message);
    }
    return a * b;
}

int isnumber(const char *string)
{
    int i = 0;
    const int l = strlen(string);
    const char isneg = l > 1 && string[0] == '-';
    if (isneg)
        ++i;

    for (i; i < l; ++i)
        if (!isdigit(string[i]))
            return 0;

    return 1;
}

char cpred(char c)
{
    return isalnum(c) || c == '-' || c == '+' || c == '*' || c == '(' || c == ')';
}

char handle_token(stack *st, char *dirty_token)
{
    char *t = clean_token(dirty_token, strlen(dirty_token), &cpred);
    int tlen = strlen(t);
    char status = 1;

    if (isnumber(t) == 1)
        status = spush(st, atol(t));
    else if (tlen == 1)
    {
        switch (t[0])
        {
        case '+':
            status = bin_op_stack(st, &fadd);
            break;
        case '-':
            status = bin_op_stack(st, &fminus);
            break;
        case '*':
            status = bin_op_stack(st, &ftimes);
            break;
        default:
            status = 0;
        }
    }
    else
        status = 0;

    free(t);
    return status;
}