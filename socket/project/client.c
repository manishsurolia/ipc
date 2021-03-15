#include "RTM.h"

int main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int i;
    int ret;
    int data_socket;
    char buffer[BUFFER_SIZE];
    routing_table * local_rtm = NULL;

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

    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        if (read(data_socket, buffer, BUFFER_SIZE) == -1) {
            perror("read");
            exit (EXIT_FAILURE);
        }
        local_rtm = (routing_table *) buffer;
        for (int i=0; i<local_rtm->count; i++) {
            printf("\n\nReceived from server : %s, %s, %s, %d\n",
                    local_rtm->rt_internal[i].oif,
                    local_rtm->rt_internal[i].destination,
                    local_rtm->rt_internal[i].gateway_ip,
                    local_rtm->rt_internal[i].mask);
        }
    }

    /* Close socket */

    close (data_socket);
    exit (EXIT_SUCCESS);
}
