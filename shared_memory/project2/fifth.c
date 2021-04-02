#include <stdio.h>
#include <sys/mman.h>
#define PAGESIZE 4096

int main(int argc, char **argv)
{
    unsigned char * first = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);


    unsigned char * second = mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    printf("First : %p\n",first);
    printf("Second : %p\n",second);
}
