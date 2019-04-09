#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "commands.h"

// exit code
static int exit_code = -1;

int main(int argc, char *argv[]) {
    while (exit_code < 0) {
        write(1, "PS1: ", 5);

        char buf[30];
        // safety measure
        buf[29] = '\0';
        // supress the EOL (\n)
        int nb_read = read(0, &buf, 29);
        buf[nb_read-1] = '\0';
        execlp(buf, buf, NULL);
    }

    return exit_code;
}
