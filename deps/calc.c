#include "calc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "edutils.h"
#include "stack.h"

static int isnumber(const char *string);
static char bin_op_stack(stack *stack, int (*f)(int, int));
static int fadd(int a, int b);
static int fminus(int a, int b);
static int ftimes(int a, int b);
static char handle_token(stack *stack, char *dirty_token);
static char handle_op(stack *st, char op);
static char cpred(char c);

#define BUFSIZE 1024

#define CHECK_PRINT(X) ({int32_t __val=(X); (__val ==-1 ? \
({fprintf(stderr,"ERROR (" __FILE__":%d) -- %s\n",__LINE__,strerror(errno));\
-1;}) : __val); })
void (*EXT_CALLBACK)(const char const *message) = NULL;

int calculate(char *unprocessed_input, int *result, void (*MSG_CALLBACK)(const char const *message)) {
        char input[BUFSIZE];
        char status;
        char *t;
        stack *st;

        EXT_CALLBACK = MSG_CALLBACK;
        memset(input, 0, BUFSIZE);
        strncpy(input, unprocessed_input, BUFSIZE);
        st = sinit();
        t = strtok(input, " ");

        while (t) {
                if (status = handle_token(st, t))
                        goto end;
                t = strtok(NULL, " ");
        }

        if (ssize(st) != 1) {
                status = EXIT_FAILURE;
                goto end;
        }

        sprint(st);

        status = spop(st, result);
end:
        sprint(st);
        sfree(st);
        EXT_CALLBACK = NULL;
        return status;
}

static char handle_token(stack *st, char *dirty_token) {
        char *t;
        int tlen;
        char status;

        t = clean_token(dirty_token, strlen(dirty_token), &cpred);
        tlen = strlen(t);
        status = EXIT_FAILURE;

        if (isnumber(t)) {
                status = spush(st, atol(t));
                goto end;
        }
        if (tlen == 1) {
                status = handle_op(st, t[0]);
                goto end;
        }

        status = EXIT_FAILURE;
end:
        free(t);
        return status;
}

static char bin_op_stack(stack *st, int (*f)(int, int)) {
        if (ssize(st) >= 2) {
                int b, a;
                if (spop(st, &b)) return EXIT_FAILURE;
                if (spop(st, &a)) return EXIT_FAILURE;
                if (spush(st, (*f)(a, b))) return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
}

static int fadd(int a, int b) {
        if (EXT_CALLBACK) {
                char message[BUFSIZE];
                sprintf(message, "%d + %d\n", a, b);
                EXT_CALLBACK(message);
        }
        return a + b;
}

static int fminus(int a, int b) {
        if (EXT_CALLBACK) {
                char message[BUFSIZE];
                sprintf(message, "%d - %d\n", a, b);
                EXT_CALLBACK(message);
        }
        return a - b;
}

static int ftimes(int a, int b) {
        if (EXT_CALLBACK) {
                char message[BUFSIZE];
                sprintf(message, "%d * %d\n", a, b);
                EXT_CALLBACK(message);
        }
        return a * b;
}

static int isnumber(const char *string) {
        int i = 0;
        int l = strlen(string);

        if (l > 1 && string[0] == '-')
                ++i;

        for (i; i < l; ++i)
                if (!isdigit(string[i]))
                        return 0;

        return 1;
}

static char cpred(char c) {
        return isalnum(c) || c == '-' || c == '+' || c == '*' || c == '(' || c == ')';
}

static char handle_op(stack *st, char op) {
        switch (op) {
                case '+':
                        return bin_op_stack(st, &fadd);
                case '-':
                        return bin_op_stack(st, &fminus);
                case '*':
                        return bin_op_stack(st, &ftimes);
                default:
                        return EXIT_FAILURE;
        }
}