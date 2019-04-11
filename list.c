#include <stdlib.h>
#include "list.h"

void add_list(struct list **l, void* data) {
    struct list **cur = l;
    while (*cur != NULL) {
        cur = &(*cur)->next;
    }
    *cur = malloc(sizeof(struct list));
    (*cur)->data = data;
    (*cur)->next = NULL;
}

void delete_list(struct list **l, void* data) {
    delete_list_with_fun(l, data, NULL);
}

void delete_list_with_fun(struct list **l, void* data, void (*fun)(void**)) {
    struct list **cur = l;
    while (*cur != NULL) {
        if ((*cur)->data == data) {
            struct list *old = *cur;
            if (fun != NULL)
                fun(&old->data);
            *cur = old->next;
            free(old);
            return;
        }
        cur = &(*cur)->next;
    }
}

void free_list(struct list **l) {
    free_list_with_fun(l, NULL);
}

void free_list_with_fun(struct list **l, void (*fun)(void**)) {
    struct list **cur = l;

    while (*cur != NULL) {
        struct list *old = *cur;
        if (fun != NULL)
            fun(&old->data);
        *cur = old->next;
        free(old);
    }
    l = NULL;
}
