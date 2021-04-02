/*
 * gcc reader.c -lrt -o reader
 */
#include <stdio.h>
#include <memory.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    char buff[1024];
    void *start_add;
    int fd;

    strcpy(buff, "Hello, I am Manish");

    /* get the shared memory FD */
    fd = shm_open("some_name", O_CREAT | O_RDONLY, 0660);
    ftruncate(fd, sizeof(buff));

    /* Map the shared memory with process using mmap */
    start_add = mmap(NULL, sizeof(buff), PROT_READ, MAP_SHARED,
                     fd, 0);

    memcpy(buff, start_add, sizeof(buff));
    printf("%s\n",buff);
    munmap(start_add, sizeof(buff));

    close(fd);
    return 0;
}
