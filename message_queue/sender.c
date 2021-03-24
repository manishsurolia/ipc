/*
 * gcc sender.c -lrt -o sender
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

int main(int argc, char **argv)
{
    char buffer[MSG_BUFFER_SIZE];
    int recvr_msg_fd = 0;

    if (argc <= 1) {
        printf("provide a recepient msgQ name : format </msgq-name>\n");
        return 0;
    }

    memset(buffer, 0, MSG_BUFFER_SIZE);
    printf("Enter the message to be sent to receiver %s\n",argv[1]);
    scanf("%s",buffer);

    if ((recvr_msg_fd = mq_open(argv[1], O_WRONLY | O_CREAT, 0, 0)) == -1) {
        perror("mq_open()");
        exit (EXIT_FAILURE);
    }

    if (mq_send(recvr_msg_fd, buffer, strlen(buffer)+1, 0) == -1) {
        perror("mq_send()");
        exit (EXIT_FAILURE);
    }

    mq_close(recvr_msg_fd);
    return 0;
}
