#ifndef SPAM_DETECTION_INTERFACE_H
#define SPAM_DETECTION_INTERFACE_H


///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


void signal_handler(int signalNumber);

int setup_server_socket(int port);
void handle_clients(int server_socket);
int thread_handler();
int run_server();


#endif /* SPAM_DETECTION_INTERFACE_H */