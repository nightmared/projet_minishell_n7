#include "process.h"
#include <sys/wait.h>

void free_process(struct process **p) {
    if (p == NULL || *p == NULL)
        return;

    char** freer = (*p)->cmd.words;
    while(*freer != NULL) {
        free(*freer);
        freer++;
    }
    free((*p)->cmd.words);

    free(*p);
    *p = NULL;
}

void scan_background_processes(struct list **bkg_proc) {
    struct list **l = bkg_proc;
    while (*l != NULL) {
        struct process *p = (struct process*)(*l)->data;

        int status;
        pid_t state = waitpid(p->pid, &status, WNOHANG);
        if (state < 0) {
            dprintf(STDERR_FILENO, "waitpid() failed, let's ignore that...\n");
        } else if (state > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                delete_list_with_fun(l, p, (void (*)(void*))&free_process);
            }
        }

        if (*l != NULL)
            l = &(*l)->next;
    }
}
