#ifndef SERVER_H
#define SERVER_H

#include "client_handling.h"
#include "process_handler.h"
#include <pthread.h>
#include <signal.h>

extern volatile sig_atomic_t keepRunning;

///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


void signal_handler(int signalNumber);

int setup_server_socket(int port);
void handle_clients(int server_socket);
int thread_handler();
int run_server();


#endif /* SERVER_H */

