#include "calc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edutils.h"
#include "stack.h"

static int32_t isnumber(const char *string);
static char bin_op_stack(stack *stack, int32_t (*f)(int32_t, int32_t));
static int32_t fadd(int32_t a, int32_t b);
static int32_t fminus(int32_t a, int32_t b);
static int32_t ftimes(int32_t a, int32_t b);
static char handle_token(stack *stack, char *dirty_token);
static char cpred(char c);

#define BUFSIZE 1024

void (*EXT_CALLBACK)(const char const *message) = NULL;

int32_t calculate(char *unprocessed_input, int32_t *result, void (*MSG_CALLBACK)(const char const *message)) {
        char input[BUFSIZE];
        char status;
        char *t;

        EXT_CALLBACK = MSG_CALLBACK;
        memset(input, 0, BUFSIZE);
        strncpy(input, unprocessed_input, BUFSIZE);

        stack *st = sinit();
        t = strtok(input, " ");

        while (t != NULL) {
                if (!(status = handle_token(st, t)))
                        goto end;
                t = strtok(NULL, " ");
        }

        if (ssize(st) != 1) {
                status = 0;
                goto end;
        }

        status = spop(st, result);
end:
        sprint(st);
        sfree(st);
        EXT_CALLBACK = NULL;
        return status;
}

static char bin_op_stack(stack *st, int32_t (*f)(int32_t, int32_t)) {
        char status = 0;
        if (ssize(st) >= 2) {
                int32_t b, a;
                status = spop(st, &b);
                status = spop(st, &a);
                status = spush(st, (*f)(a, b));
        }

        return status;
}

static int32_t fadd(int32_t a, int32_t b) {
        if (EXT_CALLBACK != NULL) {
                char message[BUFSIZE];
                sprintf(message, "%d + %d\n", a, b);
                EXT_CALLBACK(message);
        }
        return a + b;
}

static int32_t fminus(int32_t a, int32_t b) {
        if (EXT_CALLBACK != NULL) {
                char message[BUFSIZE];
                sprintf(message, "%d - %d\n", a, b);
                EXT_CALLBACK(message);
        }
        return a - b;
}

static int32_t ftimes(int32_t a, int32_t b) {
        if (EXT_CALLBACK != NULL) {
                char message[BUFSIZE];
                sprintf(message, "%d * %d\n", a, b);
                EXT_CALLBACK(message);
        }
        return a * b;
}

static int32_t isnumber(const char *string) {
        int32_t i = 0;
        int32_t l = strlen(string);
        char isneg = l > 1 && string[0] == '-';
        if (isneg)
                ++i;

        for (i; i < l; ++i)
                if (!isdigit(string[i]))
                        return 0;

        return 1;
}

static char cpred(char c) {
        return isalnum(c) || c == '-' || c == '+' || c == '*' || c == '(' || c == ')';
}

static char handle_token(stack *st, char *dirty_token) {
        char *t = clean_token(dirty_token, strlen(dirty_token), &cpred);
        int32_t tlen = strlen(t);
        char status = 1;

        if (isnumber(t))
                status = spush(st, atol(t));
        else if (tlen == 1)
                switch (t[0]) {
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
        else
                status = 0;

        free(t);
        return status;
}