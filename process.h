#ifndef PROCESS_H_HEADER
#define PROCESS_H_HEADER

#include "common.h"
#include "commands.h"
#include "list.h"

struct process {
    pid_t pid;
    struct command_line cmd;
};

void free_process(struct process **p);
void scan_background_processes(struct list **bkg_proc);

#endif // PROCESS_H_HEADER
