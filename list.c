#include <stdlib.h>
#include "list.h"

void add_list(struct list **l, void* data) {
    struct list **cur = l;
    if (*cur != NULL) {
        while ((*cur)->next != NULL)
            cur = &(*cur)->next;
    }
    *cur = malloc(sizeof(struct list));
    (*cur)->data = data;
    (*cur)->next = NULL;
}

void delete_list(struct list **l, void* data) {
    struct list **cur = l;
    while (*cur != NULL) {
        if ((*cur)->data == data) {
            struct list *old = *cur;
            cur = &old->next;
            free(old);
            return;
        }
        cur = &(*cur)->next;
    }
}

void free_list(struct list **l) {
    struct list *cur = *l;

    while (cur != NULL) {
        struct list *old = cur;
        cur = cur->next;
        free(old);
    }
    l = NULL;
}
