#ifndef COMMON_H_HEADER
#define COMMON_H_HEADER

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>


#define MAX_COMMAND_LENGTH 100

extern int exit_code;
extern struct list *background_processes;
// processus actuellement en cours d'exécution;
extern struct process *processus;

#endif // COMMON_H_HEADER
