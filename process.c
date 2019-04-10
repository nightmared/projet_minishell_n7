#include "process.h"

void free_process(struct process *p) {
    if (p == NULL)
        return;

    char** freer = p->cmd.words;
    while(*freer != NULL) {
        free(*freer);
        freer++;
    }
    free(p->cmd.words);
}
