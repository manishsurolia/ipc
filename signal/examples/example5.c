#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handler(int num)
{
    write(STDOUT_FILENO, "I won't die!\n", 13);
}

int main()
{
    int *p = NULL;
    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGKILL, handler);
    signal(SIGSEGV, handler);
    while (1) {
        *p = 0xFEED; /* just to crash the program to get SIGSEGV signal */
        printf("Wasting CPU cycles. pid : %d\n",getpid());
        sleep(1);
    }
}


