#include "common.h"
#include "commands.h"
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
        int res = chdir(argv[0]);
        if (res < 0) {
            dprintf(STDERR_FILENO, "Impossible d'aller dans le répertoire '%s': %s\n", argv[0], strerror(errno)); 
        }
    }
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
    cwd[PATH_MAX-1] = '\0';
    printf("Nous sommes maintenant dans %s\n", cwd);
}

char** read_input() {
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

    // tableau de mots (= valeur de sortie de la fonction)
    char** words = malloc((counter+1)*sizeof(char*));
    // délimiteur pour indiquer la fin des données
    words[counter] = NULL;

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

    return words;
}
