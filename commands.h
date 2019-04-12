#ifndef COMMANDS_H_HEADER
#define COMMANDS_H_HEADER

#include "common.h"

// toutes les commandes intégrées de base dans notre shell
struct builtin_command {
    char* command;
    // le programe appelé avec la reste des arguments de la ligne de commande
    void (*associated_command)(char** argv);
};

extern struct builtin_command builtin_commands[6];

struct command_line {
    // Tableau des mots qui composent la commande
    char** words;
    // Tâche d'arrière plan ?
    bool background_task;
    // La commande est-elle valide ?
    bool is_valid;
    // flux de données
    char *input_stream;
    char *output_stream;
    char *error_stream;
};

// print the command line
void print_command_line(struct command_line *cmd);

// Nombre de mots qui constituent la commande principale (n'inclue pas les processus derrière les "tubes")
int get_nb_words(struct command_line *cmd);

// Lire une commande
struct command_line read_input();

// Exécuter une commande native si nécessaire (retourne vrai si une commande native a été lancée)
bool exec_builtin(struct command_line *cmd);


#endif // COMMANDS_H_HEADER
