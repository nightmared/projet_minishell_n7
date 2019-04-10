#ifndef COMMANDS_H_HEADER
#define COMMANDS_H_HEADER

#include "common.h"

void command_exit_shell(char** argv);
void command_echo(char** argv);
void command_cd(char** argv);

// toutes les commandes intégrées de base dans notre shell
struct builtin_command {
    char* command;
    // le programe appelé avec la reste des arguments de la ligne de commande
    void (*associated_command)(char** argv);
};

extern struct builtin_command builtin_commands[2];

struct command_line {
    char** words;
    bool background_task;
};

// Lire une commande
struct command_line read_input();

#endif // COMMANDS_H_HEADER
