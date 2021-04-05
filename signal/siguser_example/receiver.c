/*
 * gcc reveiver.c -o reveiver
 * ./receiver
 */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler(int sig)
{
    printf("Signal %d recieved\n", sig);
}

int main(int argc, char **argv)
{
    printf("Waiting for SIGUSR1 signal. pid : %d\n",getpid());
    signal(SIGUSR1, handler);
    scanf("\n");
    return 0;
}
