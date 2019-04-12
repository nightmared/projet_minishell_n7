#include "commands.h"
#include <stdio.h>

#define MODIFIER(buffer, pos, old_pos) ((pos == old_pos && (buffer[pos] == '|' || buffer[pos] == '<' || buffer[pos] == '>')) || (pos == old_pos+1 && buffer[pos-1] == '2' && buffer[pos] == '>'))

void print_command_line(struct command_line *cmd) {
    char** iter = cmd->words;
    printf("%s", *iter++);
    while (*iter != NULL)
        printf(" %s", *iter++);
    putchar('\n');
}

int get_nb_words(struct command_line *cmd) {
    if (cmd == NULL);
        return 0;
    char **cur = cmd->words;
    int res = 0;
    while (*cur++ != NULL)
        res++;
    return res;
}

struct command_line read_input() {
    struct command_line res;
    // ligne de commande invalide, retournée si nécessaire
    res.is_valid = false;

    char buf[MAX_COMMAND_LENGTH];
    if (fgets(buf, MAX_COMMAND_LENGTH, stdin) == NULL) {
        printf("\nFin de fichier atteinte, au revoir !\n");
        // on quitte le shell
        exit_code = 0;
        return res;
    }
    // mesure de sécurité
    buf[MAX_COMMAND_LENGTH-1] = '\0';

    int nb_read = strlen(buf);

    // On supprime le EOL (\n)
    if (buf[--nb_read] == '\n')
        buf[nb_read] = '\0';


    if (nb_read < 2) {
        return res;
    }

    // tableau de mots
    char** words = calloc(1, sizeof(char*));

    // compte le nombre de mots et découpe simultanément la chaîne en mots
    int buf_pos = 0;
    int old_pos = 0;
    int word_number = 0;
    while (buf_pos <= nb_read) {
        if (buf[buf_pos] == ' ' || MODIFIER(buf, buf_pos, old_pos) || buf[buf_pos] == '\0') {
            if (buf_pos > old_pos || MODIFIER(buf, buf_pos, old_pos)) {
                int past_end_pos = buf_pos-old_pos;
                if MODIFIER(buf, buf_pos, old_pos)
                    past_end_pos++;
                words[word_number] = calloc((past_end_pos+1), sizeof(char));
                memcpy(words[word_number], buf+old_pos, past_end_pos);
                words[word_number][past_end_pos] = '\0';
                word_number++;
                words = realloc(words, (word_number+1)*sizeof(char*));
            }
            old_pos = buf_pos+1;
        }
        buf_pos++;
    }
    // délimiteur pour indiquer la fin des données
    words[word_number--] = NULL;

    // tâche de fond
    res.background_task = (words[word_number][0] == '&');
    if(res.background_task) {
        free(words[word_number]);
        words[word_number] = NULL;
    }

    res.words = words;
    res.is_valid = true;
    res.input_stream = stdin;
    res.output_stream = stdout;
    res.error_stream = stderr;

    // vérification des flux de donnée
    for (int i = 0; i < word_number-1; i++) {
        //if (words[i] == '>')

    }
    print_command_line(&res);


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
