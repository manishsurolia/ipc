#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    void *first = sbrk(0);
    void *second = sbrk(4096);
    void *third = sbrk(0);

    printf("First : %p\n", first);
    printf("Second : %p\n", second);
    printf("Third : %p\n", third);
    return 0;
}
