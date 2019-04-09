#ifndef COMMANDS_H_HEADER
#define COMMANDS_H_HEADER

void command_exit_shell(int argc, char** argv);


// toutes les commandes intégrées de base dans notre shell
struct builtin_command {
    char* command;
    // le programe appelé avec la reste des arguments de la ligne de commande
    void (*associated_command)(int argc, char** argv);
};

static struct builtin_command whitelisted_commands[] = {
    {"exit", command_exit_shell},

};

#endif // COMMANDS_H_HEADER
