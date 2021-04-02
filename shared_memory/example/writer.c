/*
 * gcc writer.c -lrt -o writer
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
    fd = shm_open("some_name", O_CREAT | O_RDWR | O_TRUNC, 0660);
    ftruncate(fd, sizeof(buff));

    /* Map the shared memory with process using mmap */
    start_add = mmap(NULL, sizeof(buff), PROT_WRITE, MAP_SHARED,
                     fd, 0);

    memset(start_add, 0, sizeof(buff));
    memcpy(start_add, buff, sizeof(buff));
    munmap(start_add, sizeof(buff));

    close(fd);
    return 0;
}
