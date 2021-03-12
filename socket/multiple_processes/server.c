#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main(void)
{
    struct sockaddr_un name;
    int ret;
    int connection_socket;
    int data_socket;
    int result;
    int data;
    char buffer[BUFFER_SIZE];

    /*
     * This is important, otherwise you wont be able to open a socket
     */
    unlink(SOCKET_NAME);

    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * Fill the data in sockaddr_un structure variable 'name'.
     */
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path)-1);

    /*
     * bind() call.
     */
    if (bind(connection_socket, (const struct sockaddr *) &name,
                sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    /*
     * listen() call. It can serve maximum of 20 different clients.
     */
    if (listen(connection_socket, 20) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    /*
     * Outer loop to wait for multiple clients.
     * Note : Here, server is not serving multiple clinet at same time.
     * If there is more than one client requests, it stores those requests in a
     * queue and serve one by one.
     */
    while (1) {
        data_socket = accept (connection_socket, NULL, NULL);

        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Connection established with a client.\n");

        result = 0;
        for (;;) {
            memset(buffer, 0, BUFFER_SIZE);
            if (read (data_socket, buffer, BUFFER_SIZE) == -1) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            memcpy(&data, buffer, sizeof(int));
            if (data == 0) {
                break;
            } else {
                result += data;
            }
        }

        /*
         * Send the result back to client.
         */

        memset(buffer, 0, BUFFER_SIZE);
        sprintf(buffer, "Result = %d",result);
        if (write(data_socket, buffer, BUFFER_SIZE) == -1) {
            perror ("write");
            exit (EXIT_FAILURE);
        }
        printf("Response sent back to the client\n");

        /*close socket*/
        close(data_socket);
    }

    /* close the master socket */
    close (connection_socket);
    printf("Connection Closed.\n");
    unlink (SOCKET_NAME);
    exit (EXIT_SUCCESS);
}
