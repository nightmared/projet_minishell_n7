#ifndef PROCESS_H_HEADER
#define PROCESS_H_HEADER

#include "commands.h"

struct process {
    pid_t pid;
    struct command_line cmd;
};

void free_process(struct process *p);

#endif // PROCESS_H_HEADER
