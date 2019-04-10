#include "commands.h"
#include "process.h"
#include <sys/wait.h>

int exit_code = -1;
struct list *background_processes = NULL;

int main(int argc, char *argv[]) {
    while (exit_code < 0) {
        write(STDOUT_FILENO, "PS1: ", 5);

       struct command_line input = read_input();
       struct process processus;
       processus.cmd = input;
       if (input.words[0] == NULL) {
           dprintf(STDERR_FILENO, "Pas de commande à exécuter !?\n");
           continue;
       }

       bool is_builtin = false;

        // On vérifie que la commande spécifiée ne correspond à aucune commande "native" (fournie par le shell)
        for (int i = 0; i < sizeof(builtin_commands)/sizeof(struct builtin_command); i++) {
            int max_len = strlen(builtin_commands[i].command);
            if (strlen(input.words[0]) < max_len)
                max_len = strlen(input.words[0]);
            if (!strncmp(input.words[0], builtin_commands[i].command, max_len)) {
                // Il s'agit d'une commande native, exécutons la
                (builtin_commands[i].associated_command)(&input.words[1]);

                is_builtin = true;
                break;
            }
        }
        // Est-ce une commande native ? Si c'est le cas, elle a déjà été exécutée dans la boucle avant d'arriver ici
        if (!is_builtin) {
            // On exécute la commande en cherchant dans le path
            pid_t pid = fork();
            if (pid < -1) {
                dprintf(STDERR_FILENO, "Impossible de forker, problème de mémoire ?\n");
                exit(1);
            } else if (pid == 0) {
                execvp(input.words[0], input.words);
            } else {
                processus.pid = pid;

                // tâche de fond ? on continue sans attendre...
                if (!processus.cmd.background_task)
                    continue;

                // on attend que le processus change d'état
                int wait_status;
                if (waitpid(pid, &wait_status, 0) < 0) {
                    dprintf(STDERR_FILENO, "Impossible d'attendre le processus enfant\n");
                    exit(1);
                }
            }
        }

        // on libère la mémoire allouée lorsque le processus se ferme
        if (!processus.cmd.background_task) {
            free_process(&processus);
        }
    }

    return exit_code;
}
