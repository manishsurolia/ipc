#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    void *program_break = NULL;
    int *ptr;

    for (int i=0; i<5; i++) {
        program_break = sbrk(0);
        printf("Address(sbrk(0)) : %p\n", program_break);
    }
    /* Crash : Writing on the program break(Heap boundary). */
    *(int *)program_break = 0xDEAD;
    return 0;
}
