#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    while (1) {
        printf("Wasting CPU cycles. pid : %d\n",getpid());
        sleep(1);
    }
}


