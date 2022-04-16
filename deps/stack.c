#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

struct elem {
        STYPE data;
        struct elem *next;
};

typedef struct _stack {
        struct elem *head;
} stack;

stack *sinit() {
        stack *s;

        s = malloc(sizeof(stack));
        s->head = NULL;
        return s;
}

int ssize(stack *st) {
        struct elem *h;
        int i;

        h = st->head;
        i = 0;
        while (h) {
                i++;
                h = h->next;
        }

        return i;
}

char spush(stack *s, STYPE data) {
        struct elem *e;

        e = malloc(sizeof(struct elem));
        if (!e)
                return 0;
        e->data = data;
        e->next = s->head;
        s->head = e;

        return 1;
}

char sfull(stack *s) {
        struct elem *e;

        e = malloc(sizeof(struct elem));
        free(e);
        return e == NULL;
}

char sempty(stack *s) {
        return s->head == NULL;
}

char spop(stack *s, STYPE *x) {
        if (s->head) {
                struct elem *tmp;

                tmp = s->head;
                STYPE value = s->head->data;
                s->head = s->head->next;
                free(tmp);
                *x = value;
                return 1;
        }

        return 0;
}

void sfree(stack *s) {
        while (s->head != NULL) {
                struct elem *tmp;

                tmp = s->head;
                s->head = s->head->next;
                free(tmp);
        }

        free(s);
}

void sprint(stack *st) {
        struct elem *h;

        h = st->head;
        printf("Printing what's left in stack:\n");
        while (h != NULL) {
                printf("%d\n", (int)h->data);
                h = h->next;
        }

        printf("    End.\n");
}