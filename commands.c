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

    if (nb_read < 2) {
        struct command_line invalid;
        invalid.is_valid = false;
        return invalid;
    }

    // On supprime le EOL (\n)
    buf[--nb_read] = '\0';

    // compte le nombre de mots
    int counter = 0;
    int buf_pos = 0;
    int old_pos = 0;
    while (buf_pos <= nb_read) {
        if (buf[buf_pos] == ' ' || buf[buf_pos] == '\0') {
            if (buf_pos > old_pos)
                counter += 1;
            old_pos = buf_pos+1;
        }
        buf_pos++;
    }

    // tableau de mots
    char** words = calloc(counter+1, sizeof(char*));

    struct command_line res;
    res.words = words;
    res.is_valid = true;

    // découpe la chaîne en mots
    buf_pos = 0;
    old_pos = 0;
    int word_number = 0;
    while (buf_pos <= nb_read) {
        if (buf[buf_pos] == ' ' || buf[buf_pos] == '\0') {
            if (buf_pos > old_pos) {
                words[word_number] = calloc((buf_pos-old_pos+1), sizeof(char));
                memcpy(words[word_number], buf+old_pos, buf_pos-old_pos);
                words[word_number][buf_pos-old_pos] = '\0';
                word_number++;
            }
            old_pos = buf_pos+1;
        }
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

bool exec_builtin(struct command_line *cmd) {
     for (int i = 0; i < sizeof(builtin_commands)/sizeof(struct builtin_command); i++) {
        char *cmd_name = cmd->words[0];
        int max_len = strlen(builtin_commands[i].command);
        if (strlen(cmd_name) < max_len)
            max_len = strlen(cmd_name);
        if (!strncmp(cmd_name, builtin_commands[i].command, max_len)) {
            // Il s'agit d'une commande native, exécutons la
            (builtin_commands[i].associated_command)(&cmd->words[1]);

            return true;
        }
    }
    return false;
}
