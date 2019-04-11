#ifndef PROCESS_H_HEADER
#define PROCESS_H_HEADER

#include "common.h"
#include "commands.h"
#include "list.h"

enum process_state {
    RUNNING,
    SUSPENDED
};

struct process {
    pid_t pid;
    struct command_line cmd;
    enum process_state state;
    // cette valeur ne devient vrai quel lorsque la struct est proprement complétée
    bool is_ok;
};

void wait_process_blocking(pid_t pid);
char *get_process_state(struct process *p);
void free_process(struct process **p);
void scan_background_processes(struct list **bkg_proc);

#endif // PROCESS_H_HEADER
