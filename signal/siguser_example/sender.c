/*
 * gcc sender.c -o sender
 * ./sender
 */

#include <stdio.h>
#include <signal.h>

int main(int argc, char **argv)
{
    int pid;
    printf("Enter the process id to send SIGUSR1 signal to : ");
    scanf("%d",&pid);
    kill(pid, SIGUSR1);
    return 0;
}
