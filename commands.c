#include "commands.h"
#include "process.h"
#include <stdio.h>

#define MODIFIER(buffer, pos, old_pos) ((pos == old_pos && (buffer[pos] == '|' || buffer[pos] == '<' || buffer[pos] == '>')) || (pos == old_pos+1 && buffer[pos-1] == '2' && buffer[pos] == '>'))

void print_command_line(struct command_line *cmd) {
    char** iter = cmd->words;
    printf("%s", *iter++);
    while (*iter != NULL)
        printf(" %s", *iter++);
    putchar('\n');
}


void parse_words(struct command_line *cmd, char **words, int word_number) {
    cmd->words = words;
    cmd->next_pipe = NULL;
    cmd->input_stream = NULL;
    cmd->output_stream = NULL;
    cmd->error_stream = NULL;

    // vérification des flux de données
    int i = 0;
    while (i < word_number) {
        if (words[i][0] == '>') {
            cmd->output_stream = words[i+1];
        } else if (strlen(words[i]) == 2 && words[i][0] == '2' && words[i][1] == '>') {
            cmd->error_stream = words[i+1];
        } else if (words[i][0] == '<') {
            cmd->input_stream = words[i+1];
        } else if (words[i][0] == '|') {
            free(words[i]);
            words[i] = NULL;

            cmd->next_pipe = calloc(1, sizeof(struct command_line));
            cmd->next_pipe->is_valid = false;
            parse_words(cmd->next_pipe, &words[i+1], word_number-i-1);

            break;
        } else {
            i++;
            continue;
        }
        // on libère la mémoire associés aux charactères spéciaux ('>', '<', '|'ou '2>')
        free(words[i]);
        // on décale les arguments sur la ligne de commande
        for (int j = i; j <= word_number-2; j++) {
            words[j] = words[j+2];
        }
        word_number -= 2;
        words[word_number] = NULL;
    }

    // si on est arrivé jusque ici, c'est que la parsing s'est bien déroulé, et la ligne de commande est donc_valide
    cmd->is_valid = true;
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
                if MODIFIER(buf, buf_pos, old_pos) {
                    past_end_pos++;
                }
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
    words[word_number] = NULL;

    // devons nous exécuter ce processus comme une tâche de fond ?
    res.background_task = (words[word_number-1][0] == '&');
    if(res.background_task) {
        free(words[word_number--]);
        words[word_number] = NULL;
    }

    // on analyse la commande pour détecter la présence de redirections et/ou de pipes
    parse_words(&res, words, word_number);

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
