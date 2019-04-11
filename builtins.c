#include "commands.h"
#include "list.h"
#include "process.h"
#include <signal.h>
#include <limits.h>
#include <errno.h>

void command_exit_shell(char** argv) {
    if (argv[0] != NULL) {
        exit_code = atoi(argv[0]);
    } else {
        exit_code = 0;
    }    
}

void command_echo(char** argv) {
    //TODO
    //dprintf(STDERR_FILENO, "Commande invalide.\n");
}

void command_cd(char** argv) {
    if (argv[0] != NULL) {
        char *dir = argv[0];

        // on va dans $HOME
        if (dir[0] == '~') {
            dir = getenv("HOME");
            if (dir == NULL) {
                dprintf(STDERR_FILENO, "$HOME n'est pas défini, on reste dans le répertoire courant\n");
                return;
            }
        }

        // il est temps de changer de répertoire !
        int res = chdir(dir);
        if (res < 0) {
            dprintf(STDERR_FILENO, "Impossible d'aller dans le répertoire '%s': %s\n", dir, strerror(errno)); 
        }
        printf("Nous sommes maintenant dans ");
    }
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    cwd[PATH_MAX-1] = '\0';
    printf("%s\n", cwd);
}

void command_jobs(char** argv) {
    struct list **cur = &background_processes;
    int pos = 1;
    while (*cur != NULL) {
        struct process *p = (*cur)->data;
        printf("Tâche %i: processus %i (%s) - ", pos, p->pid, get_process_state(p));
        print_command_line(&p->cmd);
        cur = &(*cur)->next;
        pos++;
    }
    if (pos == 1)
        printf("Pas de tâches concurrentes\n");

}

void command_stop(char** argv) {
    int counter = 0;
    if (argv[0] != NULL) {
        // On considère qu'on veut arrêter le premier processus
        counter = abs(atoi(argv[0]))-1;
    }
    struct list **cur = &background_processes;
    while (*cur != NULL) {
        if (counter == 0) {
            struct process *p = (*cur)->data;
            kill(p->pid, SIGSTOP);
            p->state = SUSPENDED;
            return;
        }
        counter--;
        cur = &(*cur)->next;
    }
}

void command_fg(char** argv) {
    int counter = 0;
    if (argv[0] != NULL) {
        counter = abs(atoi(argv[0]))-1;
    }
    struct list **cur = &background_processes;
    while (*cur != NULL) {
        if (counter == 0) {
            struct process *p = (*cur)->data;
            free_process(&processus);
            // on recommence l'exécution du programme
            processus = p;
            delete_list(cur, p);
            kill(p->pid, SIGCONT);
            wait_process_blocking(p->pid);
            return;
        }
        counter--;
        cur = &(*cur)->next;
    }
}

void command_bg(char** argv) {
    int counter = 0;
    if (argv[0] != NULL) {
        counter = abs(atoi(argv[0]))-1;
    }
    struct list **cur = &background_processes;
    while (*cur != NULL) {
        if (counter == 0) {
            struct process *p = (*cur)->data;
            p->state = RUNNING;
            // on recommence l'exécution du programme
            kill(p->pid, SIGCONT);
            return;
        }
        counter--;
        cur = &(*cur)->next;
    }
}

struct builtin_command builtin_commands[] = {
    {"exit", command_exit_shell},
    {"cd", command_cd},
    {"jobs", command_jobs},
    {"stop", command_stop},
    {"fg", command_fg},
    {"bg", command_bg},
};
