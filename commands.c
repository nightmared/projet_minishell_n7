#include "commands.h"
#include <limits.h>
#include <errno.h>

struct builtin_command builtin_commands[] = {
    {"exit", command_exit_shell},
    {"cd", command_cd},
};

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

struct command_line read_input() {
    char buf[MAX_COMMAND_LENGTH];
    int nb_read = read(STDIN_FILENO, &buf, MAX_COMMAND_LENGTH);
    // On supprime le EOL (\n)
    if (nb_read > 0)
        buf[--nb_read] = '\0';
    // Mesure de sécurité
    buf[MAX_COMMAND_LENGTH-1] = '\0';

    // compte le nombre de mots
    int counter = 1;
    int buf_pos = 0;
    while (buf_pos < nb_read) {
        if (buf[buf_pos++] == ' ') {
            counter += 1;
        }
    }

    // tableau de mots
    char** words = malloc((counter+1)*sizeof(char*));

    struct command_line res = { words };

    // découpe la chaîne en mots
    buf_pos = 0;
    int previous_pos = buf_pos;
    int word_number = 0;
    while (buf_pos < nb_read) {
        while (buf[buf_pos] != ' ' && buf[buf_pos] != '\0')
            buf_pos++;

        if (buf_pos != previous_pos) {
            words[word_number] = malloc((buf_pos-previous_pos+1)*sizeof(char));
            memcpy(words[word_number], buf+previous_pos, buf_pos-previous_pos+1);
            words[word_number][buf_pos-previous_pos] = '\0';
            previous_pos = buf_pos+1;
        }

        word_number++;
        buf_pos++;
    }
    // délimiteur pour indiquer la fin des données
    words[counter] = NULL;

    // tâche de fond
    res.background_task = (words[counter-1][0] == '&');
    if(res.background_task) {
        free(words[counter-1]);
        words[counter-1] = NULL;
    }


    return res;
}
