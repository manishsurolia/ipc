#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <unistd.h>

static void signal_handler (int signal, siginfo_t *si, void *arg)
{
	// Not doing much here. just returning back to the caller.
	// printf or any similar function calls are not advised here.
}

int main(void)
{
    struct sigaction sa;

    /*
        The sigaction structure is defined as something like:
           struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t sa_mask;
               int      sa_flags;
               void     (*sa_restorer)(void);
           };

        Note : There are two types of signal handlers here. One recevies only
               the 'signum' and other one receives 3 arguments (signum, siginfo,
               and the context of the program which generated this signal.

               If you want to recive a singal with all three arguments, you need
               to set SA_SIGINFO in the flags during signal registration.
    */

    // Create a signal handler and register it.
    memset(&sa, 0, sizeof(sa));       // Clear the sigaction var.
    sigemptyset(&sa.sa_mask);         // make the signal set.
    sigfillset(&sa.sa_mask);          // Initialize signal set(sigset_t) to
                                      // full. Including all signals.

    sa.sa_sigaction = signal_handler; // Set the signal action handler.
    sa.sa_flags = SA_SIGINFO;         // Upon receving of signal, get the
                                      // program context.

    sigaction(SIGSEGV, &sa, NULL);    // Register it for segment violation
                                      // signal.  

    while (1) {
        sleep(1);
        printf("Running in an infinite loop\n");
    }

    return 0;
}
