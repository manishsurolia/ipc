#define _GNU_SOURCE /* Bring REG_XXX names from /usr/include/sys/ucontext.h */

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <ucontext.h>
#include <string.h>
#include <semaphore.h>

sem_t sem_t1, sem_t2;

static void signal_handler (int signal, siginfo_t *si, void *arg)
{
    ucontext_t *ctx = (ucontext_t *)arg;

    /*
       We are on linux x86, the returning IP is stored in RIP (64bit) or
       EIP (32bit).

       In this example, the length of the offending instruction is 6 bytes.
       So we skip the offender !

       This approach is certainly not advisable, particularly in a
       multi-threaded setting. Consider a scenario where a signal was raised by
       a thread or another process managed by this thread. This handler simply
       advances the instruction pointer by 6 bytes, regardless of whether the
       instruction was problematic or not.

       It is done here just to showcase the default behviour when each thread
       is raising a SIGSEGV signal.
    */

#if __WORDSIZE == 64
        ctx->uc_mcontext.gregs[REG_RIP] += 6;
#else
        ctx->uc_mcontext.gregs[REG_EIP] += 6;
#endif
}

void *thread_func (void *)
{
    sem_wait(&sem_t2);
    /* Generate a seg fault */
    *(int *)NULL = 0; // Illegal memory access.
    printf("Continute with normal flow in thread T1\n");
}

int main(void)
{
    struct sigaction sa;
    pthread_t t;

    // Main thread should raise a SIGSEGV first, then the second thread
    sem_init(&sem_t1, 0, 1);
    sem_init(&sem_t2, 0, 0);

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

    pthread_create(&t, NULL, thread_func, NULL);

    sem_wait(&sem_t1);
    /* Generate a seg fault */
    *(int *)NULL = 0; // Illegal memory access.
    printf("Continute with normal flow in main thread\n");
    sem_post(&sem_t2);

    pthread_join(t, NULL);
    return 0;
}
