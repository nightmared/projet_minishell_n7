#ifndef LIST_H_HEADER
#define LIST_H_HEADER

struct list {
    void* data;
    struct list *next;
};

void add_list(struct list **l, void* data);
void delete_list(struct list **l, void* data);
// supprime une donnée en appelant la fonction fun sur la donnée supprimée
void delete_list_with_fun(struct list **l, void* data, void (*fun)(void**));
void free_list(struct list **l);
// libère la liste en appelant la fonction fun sur chaque donnée
void free_list_with_fun(struct list **l, void (*fun)(void**));

#endif // LIST_H_HEADER
