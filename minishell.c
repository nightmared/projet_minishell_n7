#include "common.h"
#include "commands.h"
#include "process.h"
#include "list.h"
#include "signals.h"

int exit_code = -1;
struct list *background_processes = NULL;
struct process *processus = NULL;

int main(int argc, char *argv[]) {
    register_signals();

    // un petit plus ;)
    setvbuf(stdout, NULL, _IONBF, 0);

    while (exit_code < 0) {
        printf("PS1: ");

        struct command_line input = read_input();

        // on vérifie qu'aucune tâche de fond n'a terminée
        scan_background_processes(&background_processes);

        if (!input.is_valid) {
           putchar('\n');
           continue;
        }
        if (input.words[0] == NULL) {
           dprintf(STDERR_FILENO, "Pas de commande à exécuter !?\n");
           continue;
        }

        processus = calloc(1, sizeof(struct process));
        if (processus == NULL) {
            dprintf(STDERR_FILENO, "Impossible d'allouer de la mémoire ?\n");
            exit(1);
        }
        processus->cmd = input;

        // On vérifie que la commande spécifiée ne correspond à aucune commande "native" (fournie par le shell)
        bool is_builtin = exec_builtin(&input);
        if (is_builtin) {
            free_process(&processus);
            continue;
        }

        // nouveau processus fils
        pid_t pid = fork();
        if (pid < -1) {
            dprintf(STDERR_FILENO, "Impossible de forker, problème de mémoire ?\n");
            exit(1);
        } else if (pid == 0) {
            // exécution dans un nouveau groupe de processus pour s'isoler des signaux envoyés par le shell externe (probablement bash)
            setpgid(getpid(), getpid());
            return run_with_pipe(&processus->cmd);
        } else {
            // cf. le code de l'enfant
            // ce second appel est conçu pour diminuer les 'race conditions' entre le début de l'exécution du programme et son appel à setpgid()
            setpgid(pid, pid);
            processus->pid = pid;
            processus->state = RUNNING;
            processus->is_ok = true;

            // tâche de fond ? on continue sans attendre...
            if (processus->cmd.background_task) {
                add_list(&background_processes, processus);
            } else {
                // on attend que le processus se termine
                wait_process_blocking();

                // on libère la mémoire allouée
                free_process(&processus);
            }
        }
        processus = NULL;

    }

    free_list_with_fun(&background_processes, (void (*)(void **))&free_process);

    return exit_code;
}
