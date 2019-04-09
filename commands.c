#include "common.h"
#include "commands.h"

void command_exit_shell(int argc, char** argv) {
    if (argc == 1) {
        exit_code = atoi(argv[0]);
    } else {
        dprintf(STDERR_FILENO, "Invalid command, try again !\n");
    }    
}
