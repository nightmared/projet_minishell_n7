#include "commands.h"
#include "process.h"
#include "list.h"
#include <sys/wait.h>
#include <signal.h>

int exit_code = -1;
struct list *background_processes = NULL;

void sig_handler_sigint(int signum, siginfo_t *sig_infos, void* _unused) {
    exit_code = 0;
}

int main(int argc, char *argv[]) {
    // On attrape les SIGINT (^C)
    struct sigaction signal_catcher;
    signal_catcher.sa_sigaction = &sig_handler_sigint;
    sigemptyset(&signal_catcher.sa_mask);
    signal_catcher.sa_flags = SA_SIGINFO;
    signal_catcher.sa_restorer = NULL;

    sigaction(SIGINT, &signal_catcher, NULL);


    // on force stdin a être bufferisé (important pour les tests)
    setvbuf(stdin, NULL, _IOLBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);


    while (exit_code < 0) {
        printf("PS1: ");

        struct command_line input = read_input();
        if (!input.is_valid) {
           putchar('\n');
           continue;
        }

        if (input.words[0] == NULL) {
           dprintf(STDERR_FILENO, "Pas de commande à exécuter !?\n");
           continue;
        }

        struct process *processus = malloc(sizeof(struct process));
        if (processus == NULL) {
            dprintf(STDERR_FILENO, "Impossible d'allouer de la mémoire ?\n");
            exit(1);
        }
        processus->cmd = input;

        // on vérifie qu'aucune tâche de fond n'a terminée
        scan_background_processes(&background_processes);

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
            // On exécute la commande en cherchant dans le path
            execvp(input.words[0], input.words);
        } else {
            processus->pid = pid;
            processus->state = RUNNING;

            // tâche de fond ? on continue sans attendre...
            if (processus->cmd.background_task) {
                add_list(&background_processes, processus);
            } else {
                // on attend que le processus se termine
                wait_process_blocking(pid);

                // on libère la mémoire allouée
                free_process(&processus);
            }
        }

    }

    free_list_with_fun(&background_processes, (void(*)(void**))&free_process);

    return exit_code;
}
