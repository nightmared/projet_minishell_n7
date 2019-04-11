#ifndef COMMON_H_HEADER
#define COMMON_H_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_COMMAND_LENGTH 30

extern int exit_code;
extern struct list *background_processes;

#endif // COMMON_H_HEADER
