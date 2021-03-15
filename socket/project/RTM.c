#include "RTM.h"
routing_table rt;

void initialize_monitor_fd_set(int *monitored_fd_set, int max_fd_count)
{
    for(int i=0; i<max_fd_count; i++) {
       monitored_fd_set[i] = -1; 
    }
    return;
}

void add_to_monitored_fd_set(int *monitored_fd_set, int max_fd_count, int socket_fd)
{
    for (int i=0; i < max_fd_count; i++) {
        if (monitored_fd_set[i] != -1) {
            continue;
        }
        monitored_fd_set[i] = socket_fd;
        break;
    }
    return;
}

void remove_from_monitored_fd_set(int *monitored_fd_set, int max_fd_count, int socket_fd)
{
    for (int i=0; i < max_fd_count; i++) {
        if (monitored_fd_set[i] != socket_fd) {
            continue;
        }
        monitored_fd_set[i] = -1;
        break;
    }
    return;
}

void refresh_fd_set(int *monitored_fd_set, int max_fd_count, fd_set *fd_set_ptr)
{
    FD_ZERO(fd_set_ptr);
    for (int i=0; i < max_fd_count; i++) {
       if (monitored_fd_set[i] != -1) {
           FD_SET(monitored_fd_set[i], fd_set_ptr);
       } 
    }
    return;
}

int get_max_fd(int *monitored_fd_set, int max_fd_count)
{
    int max = -1;
    for (int i=0; i < max_fd_count; i++) {
        if (monitored_fd_set[i] > max) {
            max = monitored_fd_set[i];
        }
    }
    return max;
}

void dump_existing_table_to_client(int data_socket)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    rt.count = 1;
    strcpy(rt.rt_internal[0].oif,"e0/0");
    strcpy(rt.rt_internal[0].destination, "1.2.3.4");
    strcpy(rt.rt_internal[0].gateway_ip, "12.34.56.78");
    rt.rt_internal[0].mask = 10;
    memcpy(buffer, &rt, sizeof(rt));
    if (write(data_socket, buffer, sizeof(buffer)) == -1) {
        perror("write");
        return;
    }
    memset(buffer, 0, sizeof(buffer));
    rt.count = 2;
    strcpy(rt.rt_internal[1].oif,"e0/1");
    strcpy(rt.rt_internal[0].destination, "11.22.33.44");
    strcpy(rt.rt_internal[0].gateway_ip, "123.345.567.789");
    rt.rt_internal[0].mask = 11;
    memcpy(buffer, &rt, sizeof(rt));
    if (write(data_socket, buffer, sizeof(buffer)) == -1) {
        perror("write");
        return;
    }
    return;
}

void start_server(int connection_socket, int * monitored_fd_set, int max_clients)
{
    int ret;
    int result;
    int data;
    struct timeval tv;
    fd_set readfds;
    int comm_socket_fd, i;
    int data_socket;
    initialize_monitor_fd_set(monitored_fd_set, max_clients);

    tv.tv_sec = 15;
    tv.tv_usec = 0;

    /*
     * First, check whether there are connection requests from new clients.
     * If yes, then accept the connections and add the returned file
     * descriptors to global list for further communication.
     */
    while(1) {
        printf("\nCalling select for connection socket : %d\n",
                connection_socket);
        FD_ZERO(&readfds);
        FD_SET(connection_socket, &readfds);
        if (select(connection_socket+1, &readfds, NULL, NULL, &tv) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (*(int *)&readfds == 0) {
            /*
             * Timeout : as per passed tv var.
             * This means, we dont have any more activities on the connection
             * file descriptor.
             * Means, no new connection requests.
             * stop the loop and send routing table to all the clients.
             */
            break;
        } else if (FD_ISSET(connection_socket, &readfds)) {
            printf("\nGot a connection request from a client\n");
            if ((data_socket = accept(connection_socket, NULL, NULL)) == -1) {
                perror("accpet");
                exit(EXIT_FAILURE);
            }
            printf("\nAdding fd : %d the global fd list for new client\n",
                    data_socket);
            add_to_monitored_fd_set(monitored_fd_set, max_clients,
                    data_socket);
            dump_existing_table_to_client(data_socket);
        }
    }

    /*
     * Second, go through all the client data sockets and send them the data we
     * have currently.
     */

    return;
}

void stop_server(int connection_socket, int *client_fds)
{
    close(connection_socket);
    free(client_fds);
    unlink (SOCKET_NAME);
}

int init_server(int **client_fds, int max)
{
    struct sockaddr_un name;
    int connection_socket;
    int *temp_client_fds;
    unlink(SOCKET_NAME);

    if ((*client_fds = malloc(sizeof(int) * max)) == NULL) {
        perror("malloc");
        return -1;
    }

    /*
     * Initialize all client fd set to -1.
     */
    temp_client_fds = *client_fds;
    for (int i = 0; i<max; i++) {
        temp_client_fds[i] = -1; 
    }

    /*
     * Create a connection socket.
     */
    if ((connection_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    /*
     * Set and use the name ('struct sockaddr_un') to bind with connection 
     * socket.
     */  
    memset(&name, 0, sizeof(struct sockaddr_un));
    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1); 

    if (bind(connection_socket, (const struct sockaddr *) &name,
                sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        return -1;
    }

    /*
     * Set the connection socket for listening.
     */
    if (listen(connection_socket, MAX_CLIENT_SUPPORTED) == -1) {
        perror("listen");
        return -1;
    }
    return connection_socket;
}

int main(int argc, char **argv)
{
    int connection_socket;
    int *client_fds;

    /*
     * Init server metadata.
     */
    if ((connection_socket =
                init_server(&client_fds, MAX_CLIENT_SUPPORTED)) == -1) {
        printf("Server initialization failed\n");
    }

    /*
     * Start the server.
     */
    start_server(connection_socket, client_fds, MAX_CLIENT_SUPPORTED);

    /*
     * Stop the server.
     */
    stop_server(connection_socket, client_fds);
    client_fds = NULL;

    return 0;
}
