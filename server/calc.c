#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "calc.h"
#include "stack.h"
int isnumber(const char *string);
void bin_op_stack(Stack *stack, long (*f)(long, long));
long fadd(long a, long b);
long fminus(long a, long b);
long ftimes(long a, long b);
char handle_token(Stack *stack, char *dirty_token);
char cpred(char c);

#define BUFSIZE 1024

void (*EXT_CALLBACK)(const char const *message) = NULL;

int calculate(char *unprocessed_input, long *result, void (*MSG_CALLBACK)(const char const *message))
{
    EXT_CALLBACK = MSG_CALLBACK;

    char input[BUFSIZE];
    bzero(input, BUFSIZE);
    strcpy(input, unprocessed_input);

    Stack st = sinit();
    char *tok = strtok(input, " ");

    while (tok != NULL)
    {
        if (!handle_token(&st, tok))
            return 0;
        tok = strtok(NULL, " ");
    }

    const char result_status = spop(&st, result);

    sprint(st);
    sfree(&st);
    EXT_CALLBACK = NULL;
    return result_status;
}

void bin_op_stack(Stack *st, long (*f)(long, long))
{
    if (ssize(st->head) >= 2)
    {
        long b;
        spop(st, &b);
        long a;
        spop(st, &a);
        spush(st, (*f)(a, b));
    }
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

char handle_token(Stack *st, char *dirty_token)
{
    const char *t = clean_token(dirty_token, strlen(dirty_token), &cpred);
    const int tlen = strlen(t);
    char status = 1;

    if (isnumber(t) == 1)
        spush(st, atol(t));
    else if (tlen == 1)
    {
        if (t[0] == '+')
            bin_op_stack(st, &fadd);
        else if (t[0] == '-')
            bin_op_stack(st, &fminus);
        else if (t[0] == '*')
            bin_op_stack(st, &ftimes);
        else
            status = 0;
    }
    else
        status = 0;

    free((void *)t);
    return status;
}