#include <stdio.h>
#include <stdlib.h>

#define ONEGB (1<<30)

int main(int argc, char **argv)
{
    int count = 0;

    while(1) {
        if (malloc(ONEGB) == NULL) {
            printf("malloc refused after %d GB\n", count);
            return 0;
        }
        printf("Got %d GB\n",++count);
    }
}
