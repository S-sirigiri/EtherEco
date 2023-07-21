#ifndef SERVER_H
#define SERVER_H

#include "client_handling.h"
#include <pthread.h>
#include <signal.h>

///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


void signal_handler(int signalNumber);

int setup_server_socket(int port);
void handle_clients(int server_socket);
int thread_handler();
int run_server();


#endif /* SERVER_H */

