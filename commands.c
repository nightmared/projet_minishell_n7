#include "commands.h"
#include <stdio.h>

void print_command_line(struct command_line *cmd) {
    char** iter = cmd->words;
    printf("%s", *iter++);
    while (*iter != NULL)
        printf(" %s", *iter++);
    putchar('\n');
}

struct command_line read_input() {
    // ligne de commande invalide, retournée si nécessaire
    struct command_line invalid;
    invalid.is_valid = false;

    char buf[MAX_COMMAND_LENGTH];
    if (fgets(buf, MAX_COMMAND_LENGTH, stdin) == NULL) {
        printf("\nFin de fichier atteinte, au revoir !\n");
        // on quitte le shell
        exit_code = 0;
        return invalid;
    }

    int nb_read = strlen(buf);

    // On supprime le EOL (\n)
    if (buf[--nb_read] == '\n')
        buf[nb_read] = '\0';


    if (nb_read < 2) {
        return invalid;
    }

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

    // délimiteur pour indiquer la fin des données, normalement inutile puisqu'on utilsie calloc
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
