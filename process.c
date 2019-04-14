#include "process.h"
#include <sys/wait.h>
#include <errno.h>

void wait_process_blocking() {
    bool stopped = false;
    int wait_status;
    while (!stopped) {
        if (waitpid(processus->pid, &wait_status, WUNTRACED) < 0) {
            // waitpid() interrompu par un signal
            if (errno == EINTR && processus != NULL) {
                continue;
            }
            dprintf(STDERR_FILENO, "Impossible d'attendre le processus enfant: %s\n", strerror(errno));
            exit(1);
        }
        // le processus a été terminé ou mis en tâche de fond par une interaction extérieure
        if (processus == NULL)
            return;
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

void free_words(char **words) {
    char** freer = words;
    while(*freer != NULL) {
        free(*freer);
        freer++;
    }
}

void free_process(struct process **p) {
    if (p == NULL || *p == NULL)
        return;

    free_words((*p)->cmd.words);

    // Libération des flux de données
    if ((*p)->cmd.input_stream != NULL)
        free((*p)->cmd.input_stream);
    if ((*p)->cmd.output_stream != NULL)
        free((*p)->cmd.output_stream);
    if ((*p)->cmd.error_stream != NULL)
        free((*p)->cmd.error_stream);

    // suppression du pipeline
    struct command_line *next_pipe = (*p)->cmd.next_pipe;
    while (next_pipe != NULL) {
        struct command_line *cur = next_pipe;
        next_pipe = cur->next_pipe;
        free_words(cur->words);
        free(cur);
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
        pid_t state = waitpid(p->pid, &status, WCONTINUED | WUNTRACED | WNOHANG);
        if (state < 0) {
            dprintf(STDERR_FILENO, "waitpid() failed, let's ignore that...\n");
        } else if (state > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                delete_list_with_fun(l, p, (void (*)(void **))&free_process);
            } else if (WIFSTOPPED(status)) {
                p->state = SUSPENDED;
            } else if (WIFCONTINUED(status)) {
                p->state = RUNNING;
            }
        }

        if (*l != NULL)
            l = &(*l)->next;
    }
}

int run_command() {
    // On mets à jour les directions des flux de données
    if (processus->cmd.output_stream) {
        if (freopen(processus->cmd.output_stream, "w", stdout) == NULL) {
            dprintf(STDERR_FILENO, "Échec de l'ouverture du fichier %s\n", processus->cmd.output_stream);
            return EXIT_FAILURE;
        }
    }
    if (processus->cmd.error_stream) {
        if (freopen(processus->cmd.error_stream, "w", stderr) == NULL) {
            dprintf(STDERR_FILENO, "Échec de l'ouverture du fichier %s\n", processus->cmd.error_stream);
            return EXIT_FAILURE;
        }
    }
    if (processus->cmd.input_stream) {
        if (freopen(processus->cmd.input_stream, "r", stdin) == NULL) {
            dprintf(STDERR_FILENO, "Échec de l'ouverture du fichier %s\n", processus->cmd.input_stream);
            return EXIT_FAILURE;
        }
    }

    // On exécute la commande en cherchant dans le path
    if (execvp(processus->cmd.words[0], processus->cmd.words) < 0) {
        dprintf(STDERR_FILENO, "Commande non trouvée dans votre $PATH\n");
        return EXIT_FAILURE;
    }

    // pour faire plaisi au compilateur, puisqu'on n'arrivera jamais jusque ici
    return EXIT_SUCCESS;
}
