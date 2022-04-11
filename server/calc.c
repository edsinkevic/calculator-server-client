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
char handle_parens(char *tok, long *res_ptr, char **strtok_save);
char cpred(char c);

#define BUFSIZE 1024

void (*EXT_CALLBACK)(const char const *message) = NULL;

void print_stack(Stack st)
{
    struct element *head = st.head;
    printf("Printing what's left in stack:\n");
    while (head != NULL)
    {
        printf("%d\n", (int)head->data);
        head = head->next;
    }

    printf("    End.\n");
}

int calculate(char *unprocessed_input, long *result, void (*MSG_CALLBACK)(const char const *message))
{
    EXT_CALLBACK = MSG_CALLBACK;

    char input[BUFSIZE];
    bzero(input, BUFSIZE);
    strcpy(input, unprocessed_input);

    Stack st = init();
    char *tok = strtok(input, " ");

    while (tok != NULL)
    {
        if (strlen(tok) >= 1 && tok[0] == '(')
        {
            long res = 0;
            if (handle_parens(tok, &res, &save))
                push(&st, res);
        }
        else if (!handle_token(&st, tok))
            return 0;
        tok = strtok(NULL, " ");
    }

    const char result_status = pop(&st, result);

    print_stack(st);
    free_stack(&st);
    EXT_CALLBACK = NULL;
    return result_status;
}
char handle_parens(char *t, long *res_ptr, char **strtok_save)
{
    if (strlen(t) != 1 || t[0] != '(')
        return 0;

    t = strtok_r(NULL, " ", strtok_save);
    int open_parens_count = 1;
    char status = 0;
    int parsize = 0;
    int lsize = 10;
    char **tarr = (char **)calloc(lsize, sizeof(char *));

    int i = 0;
    for (; t; ++i)
    {
        char *ctok = clean_token(t, strlen(t), &cpred);
        if (strlen(ctok) == 1 && ctok[0] == ')' && open_parens_count == 1)
        {
            status = 1;
            char *rbuf = (char *)calloc(parsize, sizeof(char));
            for (int j = 0; j < i; ++j)
                sprintf(rbuf, "%s %s", rbuf, tarr[j]);

            printf("[%s]", rbuf);

            calculate(rbuf, res_ptr, EXT_CALLBACK);
            free(rbuf);
            break;
        }
        else if (strlen(ctok) == 1 && ctok[0] == ')')
            open_parens_count--;
        else if (strlen(ctok) == 1 && ctok[0] == '(')
            open_parens_count++;

        if (i >= lsize)
        {
            lsize *= 2;
            tarr = (char **)realloc(tarr, sizeof(char *) * lsize);
        }
        parsize += strlen(ctok) + 1;
        tarr[i] = ctok;
        t = strtok_r(NULL, " ", strtok_save);
    }

    for (int j = 0; j < i; ++j)
        free(tarr[j]);
    free(tarr);

    return status;
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

void bin_op_stack(Stack *st, long (*f)(long, long))
{
    if (ssize(st->head) >= 2)
    {
        long b;
        pop(st, &b);
        long a;
        pop(st, &a);
        push(st, (*f)(a, b));
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

char cpred(char c)
{
    return isalnum(c) || c == '-' || c == '+' || c == '*' || c == '(' || c == ')';
}

char handle_token(Stack *st, char *dirty_token)
{
    const char *t = clean_token(dirty_token, strlen(dirty_token), &cpred);
    const int tlen = strlen(t);

    if (isnumber(t) == 1)
        push(st, atol(t));
    else if (tlen == 1)
    {
        if (t[0] == '+')
            bin_op_stack(st, &fadd);
        else if (t[0] == '-')
            bin_op_stack(st, &fminus);
        else if (t[0] == '*')
            bin_op_stack(st, &ftimes);
    }
    else
    {
        free((void *)t);
        return 0;
    }

    free((void *)t);
    return 1;
}