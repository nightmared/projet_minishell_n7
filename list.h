#ifndef LIST_H_HEADER
#define LIST_H_HEADER

struct list {
    void* data;
    struct list *next;
};

void add_list(struct list **l, void* data);
void delete_list(struct list **l, void* data);
void free_list(struct list **l);

#endif // LIST_H_HEADER
