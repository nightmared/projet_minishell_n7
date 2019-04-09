#include "commands.h"
#include "common.h"
#include <stdbool.h>
#include <sys/wait.h>

int exit_code = -1;

int main(int argc, char *argv[]) {
    while (exit_code < 0) {
        write(STDOUT_FILENO, "PS1: ", 5);

       char **words = read_input();
       if (words[0] == NULL) {
           dprintf(STDERR_FILENO, "Pas de commande à exécuter !?\n");
           continue;
       }

       bool is_builtin = false;

        // On vérifie que la commande spécifiée ne correspond à aucune commande "native" (fournie par le shell)
        for (int i = 0; i < sizeof(builtin_commands)/sizeof(struct builtin_command); i++) {
            int max_len = strlen(builtin_commands[i].command);
            if (strlen(words[0]) < max_len)
                max_len = strlen(words[0]);
            if (!strncmp(words[0], builtin_commands[i].command, max_len)) {
                // Il s'agit d'une commande native, exécutons la
                (builtin_commands[i].associated_command)(&words[1]);

                is_builtin = true;
                break;
            }
        }
        // Est-ce une commande native ? Si c'est le cas, elle a déjà été exécutée dans la boucle avant d'arriver ici
        if (is_builtin)
            continue;

        // On exécute la commande en cherchant dans le path
        pid_t pid = fork();
        if (pid < -1) {
            dprintf(STDERR_FILENO, "Impossible de forker, problème de mémoire ?\n");
            exit(1);
        } else if (pid == 0) {
            execvp(words[0], words);
        } else {
            // on attends que le processus change d'état
            int wait_status;
            if (waitpid(pid, &wait_status, 0) < 0) {
                dprintf(STDERR_FILENO, "Impossible d'attendre le processus enfant\n");
                exit(1);
            }


        }
    }

    return exit_code;
}
