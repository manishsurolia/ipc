#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_ROUTING_ENTRIES 100

typedef struct routing_table_internal
{
    char oif[32];
    char destination[16];
    char gateway_ip[16];
    char mask;
} routing_table_internal;

typedef struct routing_table
{
    unsigned int count;
    routing_table_internal rt_internal[MAX_ROUTING_ENTRIES];
} routing_table;

#define MAX_CLIENT_SUPPORTED 32
#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE (sizeof(routing_table) * MAX_ROUTING_ENTRIES)
