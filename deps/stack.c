#include "stack.h"

#include <stdio.h>
#include <stdlib.h>

struct element {
        STYPE data;
        struct element *next;
};

typedef struct _stack {
        struct element *head;
} stack;

stack *sinit() {
        stack *s = (stack *)malloc(sizeof(stack));
        s->head = NULL;
        return s;
}

int ssize(stack *st) {
        struct element *h = st->head;
        int count = 0;
        while (h != NULL) {
                count++;
                h = h->next;
        }

        return count;
}

char spush(stack *s, STYPE data) {
        struct element *node = (struct element *)malloc(sizeof(struct element));

        if (node == NULL)
                return 0;

        node->data = data;
        node->next = s->head;
        s->head = node;

        return 1;
}

char sfull(stack *s) {
        struct element *node = (struct element *)malloc(sizeof(struct element));
        free(node);
        return node == NULL;
}

char sempty(stack *s) {
        return s->head == NULL;
}

char spop(stack *s, STYPE *x) {
        if (s->head != NULL) {
                struct element *tmp = s->head;
                STYPE value = s->head->data;
                s->head = s->head->next;
                free(tmp);
                *x = value;
                return 1;
        }

        return 0;
}

void sfree(stack *s) {
        struct element *tmp;

        while (s->head != NULL) {
                tmp = s->head;
                s->head = s->head->next;
                free(tmp);
        }

        free(s);
}

void sprint(stack *st) {
        struct element *head = st->head;
        printf("Printing what's left in stack:\n");
        while (head != NULL) {
                printf("%d\n", (int)head->data);
                head = head->next;
        }

        printf("    End.\n");
}