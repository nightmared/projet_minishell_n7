#include "process.h"
#include "signals.h"
#include <sys/wait.h>
#include <errno.h>

void wait_process_blocking() {
    bool stopped = false;
    int wait_status;
    while (!stopped) {
        if (waitpid(processus->pid, &wait_status, WUNTRACED) < 0) {
            // waitpid() interrompu par un signal
            if (errno == EINTR) {
                if (processus == NULL) {
                    return;
                }
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

int run_with_pipe(struct command_line *current) {
    // traitement du pipeline
    if (current->next_pipe != NULL) {
        // Nouveau pipe pour la communication entre les deux processus
        int pipefd[2];
        // on ne fait pas de vérification et on croise les doigts pour que les ulimits soient assez élevées
        pipe(pipefd);

        pid_t pid = fork();
        if (pid < -1) {
            dprintf(STDERR_FILENO, "Impossible de forker, problème de mémoire ?\n");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            // on connecte la sortie du processus précédent avec l'entrée du nouveau processus
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);

            return run_with_pipe(current->next_pipe);
        } else {
            close(pipefd[0]);
            // on redirige notre sortie standard vers le nouveau processus
            dup2(pipefd[1], STDOUT_FILENO);
        }
    }

    // On mets à jour les directions des flux de données
    if (current->output_stream) {
        if (freopen(current->output_stream, "w", stdout) == NULL) {
            dprintf(STDERR_FILENO, "Échec de l'ouverture du fichier %s\n", current->output_stream);
            return EXIT_FAILURE;
        }
    }
    if (current->error_stream) {
        if (freopen(current->error_stream, "w", stderr) == NULL) {
            dprintf(STDERR_FILENO, "Échec de l'ouverture du fichier %s\n", current->error_stream);
            return EXIT_FAILURE;
        }
    }
    if (current->input_stream) {
        if (freopen(current->input_stream, "r", stdin) == NULL) {
            dprintf(STDERR_FILENO, "Échec de l'ouverture du fichier %s\n", current->input_stream);
            return EXIT_FAILURE;
        }
    }


    // On exécute la commande en cherchant dans le path
    if (execvp(current->words[0], current->words) < 0) {
        dprintf(STDERR_FILENO, "Commande '%s' non trouvée dans votre $PATH\n", current->words[0]);
        return EXIT_FAILURE;
    }

    // présent uniquement pour faire plaisir au compilateur, puisqu'on n'arrivera jamais jusque ici
    return EXIT_SUCCESS;
}

int run_command(struct command_line *cmd) {
    //// on masque les signaux sigtstp et sigint, malheureusement (cf. moodle)
    //sigset_t set;
    //sigemptyset(&set);
    //sigaddset(&set, SIGTSTP);
    //sigaddset(&set, SIGINT);
    //sigprocmask(SIG_SETMASK, &set, NULL);

    run_with_pipe(cmd);
    return EXIT_SUCCESS;
}
