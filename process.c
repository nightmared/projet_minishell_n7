#include "process.h"
#include <sys/wait.h>
#include <errno.h>

void wait_process_blocking() {
    if (processus == NULL)
        return;

    bool stopped = false;
    int wait_status;
    while (!stopped) {
        if (waitpid(processus->pid, &wait_status, WUNTRACED) < 0) {
            // waitpid() interrompu par un signal
            if (errno == EINTR) {
                // le processus a été terminé ou mis en tâche de fond par une interaction extérieure
                if (processus == NULL)
                    return;
                continue;
            }
            dprintf(STDERR_FILENO, "Impossible d'attendre le processus enfant: %s\n", strerror(errno));
            exit(1);
        }
        // Le processus a été stoppé par un signal
        if (WIFSTOPPED(wait_status)) {
            processus->state = SUSPENDED;
            add_list(&background_processes, processus);
            processus = NULL;
            return;
        }
        if (WIFEXITED(wait_status) || WIFSIGNALED(wait_status)) {
            stopped = true;
        }
    }
}

char *get_process_state(struct process *p) {
    if (p->state == RUNNING) {
        return "en cours d'exécution";
    } else {
        return "suspendu";
    }
}

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
                delete_list_with_fun(l, p, (void (*)(void**))&free_process);
            }
        }

        if (*l != NULL)
            l = &(*l)->next;
    }
}
