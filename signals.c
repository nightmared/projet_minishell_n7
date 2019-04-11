#include "signals.h"
#include "process.h"
#include <sys/wait.h>
#include <signal.h>

void sig_handler_sigint(int signum) {
    // propagation du signal
    if (processus != NULL && processus->is_ok)
        kill(processus->pid, SIGINT);
}

void sig_handler_sigtstp(int signum) {
    // propagation du signal
    if (processus != NULL && processus->is_ok) {
        kill(processus->pid, SIGSTOP);
        processus->state = SUSPENDED;
        add_list(&background_processes, processus);
        processus = NULL;
    }
}

void register_signals(void) {
    // On attrape les SIGINT (^C)
    struct sigaction signal_catcher;
    signal_catcher.sa_handler = &sig_handler_sigint;
    sigemptyset(&signal_catcher.sa_mask);
    sigaddset(&signal_catcher.sa_mask, SIGINT);
    sigaddset(&signal_catcher.sa_mask, SIGTSTP);
    signal_catcher.sa_flags = 0;
    signal_catcher.sa_restorer = NULL;

    sigaction(SIGINT, &signal_catcher, NULL);

    // idem pour les SIGTSTP (^Z)
    signal_catcher.sa_handler = &sig_handler_sigtstp;
    sigaction(SIGTSTP, &signal_catcher, NULL);
}
