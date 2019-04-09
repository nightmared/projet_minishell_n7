#ifndef COMMANDS_H_HEADER
#define COMMANDS_H_HEADER

void command_exit_shell(char** argv);
void command_echo(char** argv);
void command_cd(char** argv);

// toutes les commandes intégrées de base dans notre shell
struct builtin_command {
    char* command;
    // le programe appelé avec la reste des arguments de la ligne de commande
    void (*associated_command)(char** argv);
};

static struct builtin_command builtin_commands[] = {
    {"exit", command_exit_shell},
    {"cd", command_cd},

};

// Lire une commande
char** read_input();

#endif // COMMANDS_H_HEADER
