#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int i;
    int ret;
    int data_socket;
    char buffer[BUFFER_SIZE];

    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (data_socket == -1) {
        perror("socket");
        exit (EXIT_FAILURE);
    }

    memset (&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy (addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    if (connect(data_socket, (const struct sockaddr *) &addr,
            sizeof(struct sockaddr_un)) == -1) {
        fprintf(stderr, "The server is down.\n");
        exit (EXIT_FAILURE);
    }

    /*
     * Send arguments.
     */
    do {
        printf("Enter number to send to server : \n");
        scanf ("%d",&i);

        if (write(data_socket, &i, sizeof(int)) == -1) {
            perror ("write");
            break;
        }
        printf("No. of bytes sent = %d, data sent = %d\n",ret, i);
    } while (i);


    memset(buffer, 0, BUFFER_SIZE);
    if (read(data_socket, buffer, BUFFER_SIZE) == -1) {
        perror("read");
        exit (EXIT_FAILURE);
    }

    printf("Received from server : %s\n",buffer);

    /* Close socket */

    close (data_socket);
    exit (EXIT_SUCCESS);
}
