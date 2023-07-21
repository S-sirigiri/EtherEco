#ifndef CLIENT_HANDLING_H
#define CLIENT_HANDLING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <libpq-fe.h>
#include "online_record_datastruct.h"
#include "database.h"

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PORT 12346

extern int client_sockets[MAX_CLIENTS];
extern int num_clients;


///////////////////////
//FUNCTION DECLARATIONS
///////////////////////



int transact_recv_send_map_reduce(int client_socket, int (*map_reduce_function)(char*, char*), char *recv, char *send, char* error);
int transact_send_recv_map_reduce(int client_socket, int (*map_reduce_function)(char*, char*), char *recv, char *send, char* error);

int strcmp_wrapper(char* arg1, char* arg2);

void close_connection(int client_socket);
int transact_five_times(int (*func)(int, int (*)(char*, char*), char*, char*, char*), int client_socket, int (*map_reduce_function)(char*, char*), char *recv, char *send, char* error);


void *client_handler(void *arg);

#endif /* CLIENT_HANDLING_H */
