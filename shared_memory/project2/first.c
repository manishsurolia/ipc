#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    for (int i=0; i<5; i++) {
        char *ptr = malloc(50000);
        printf("Got memory! (Address = %p)\n", ptr);
    }
}
