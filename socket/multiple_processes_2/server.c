#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_CLIENT_SUPPORTED 32
#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

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


int main(int argc, char **argv)
{
    struct sockaddr_un name;

    int ret;
    int connection_socket;
    int result;
    int data;
    int max_fd;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int comm_socket_fd, i;
    int data_socket;
    int monitored_fd_set[MAX_CLIENT_SUPPORTED];
    int client_result[MAX_CLIENT_SUPPORTED] = {0};
    initialize_monitor_fd_set(monitored_fd_set, MAX_CLIENT_SUPPORTED);

    unlink(SOCKET_NAME);

    /*
     * Create a connection socket.
     */
    if ((connection_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    /*
     * Set the connection socket for listening.
     */
    if (listen(connection_socket, 20) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    add_to_monitored_fd_set(monitored_fd_set, MAX_CLIENT_SUPPORTED,
            connection_socket);

    FD_SET(connection_socket, &readfds);
    max_fd = connection_socket;
    while(1) {
        refresh_fd_set(monitored_fd_set, MAX_CLIENT_SUPPORTED, &readfds);
        select(get_max_fd(monitored_fd_set, MAX_CLIENT_SUPPORTED)+1, &readfds,
                NULL, NULL, NULL);

        if (FD_ISSET(connection_socket, &readfds)) {
            if ((data_socket = accept(connection_socket, NULL, NULL)) == -1) {
                perror("accpet");
                exit(EXIT_FAILURE);
            }    
            printf("Connection request from a new client. fd : %d\n",
                    data_socket);
            add_to_monitored_fd_set(monitored_fd_set, MAX_CLIENT_SUPPORTED,
                    data_socket);
        } else {
            int comm_socket_fd = -1;
            for (int i=0; i < MAX_CLIENT_SUPPORTED; i++) {
                if (FD_ISSET(monitored_fd_set[i], &readfds)) {
                    comm_socket_fd = monitored_fd_set[i];
                    memset(buffer, 0, sizeof(buffer));
                    if (read(comm_socket_fd, buffer, sizeof(buffer)) == -1) {
                        perror("read");
                        exit(EXIT_FAILURE); 
                    }
                    memcpy(&data, buffer, sizeof(data));
                    if (data == 0) {
                        memset(buffer, 0, sizeof(buffer));
                        sprintf(buffer, "Result = %d", client_result[i]); 
                        if (write(comm_socket_fd, buffer, sizeof(buffer)) == -1) {
                            perror("write");
                            exit(EXIT_FAILURE); 
                        }
                        close(comm_socket_fd);
                        client_result[i] = 0;
                        remove_from_monitored_fd_set(monitored_fd_set,
                                MAX_CLIENT_SUPPORTED, comm_socket_fd);
                        continue;
                    }
                    printf("adding data %d to the result for fd : %d\n",data,
                            comm_socket_fd);
                    client_result[i] += data;
                }
            } 
        } 
    }
    close(connection_socket);
    remove_from_monitored_fd_set(monitored_fd_set, MAX_CLIENT_SUPPORTED,
            connection_socket);
    unlink (SOCKET_NAME);
    exit(EXIT_SUCCESS);
}
