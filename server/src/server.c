#include "server.h"


/**
 * Signal handler function to handle SIGINT (interrupt signal from keyboard).
 *
 * @param signalNumber - The numeric representation of the signal that caused the invocation of this handler.
 */
void signal_handler(int signalNumber) {
    // Check if the signal is SIGINT (interrupt signal from keyboard)
    if (signalNumber == SIGINT) {
        printf("\nHalting the server....\n");
        // Exit the process with a status of 0 (success)
        exit(0);
    }
}



/**
 * Function to setup a server socket listening on a given port.
 *
 * @param   port   An integer specifying the port number on which the server should listen.
 * @return  An integer representing the server socket descriptor on success, -1 on failure.
 */
int setup_server_socket(int port) {
    // Create a new socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        // Print an error message if the socket creation failed
        perror("Error creating socket");
        return -1;
    }

    // Declare a variable for the server's address and initialize it to zero
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    // Set the family, address, and port in the server_addr struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    // Bind the server socket to the specified port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        // Print an error message if the binding failed
        perror("Error binding socket");
        return -1;
    }

    // Start listening for connections on the server socket
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        // Print an error message if the listen call failed
        perror("Error listening for connections");
        return -1;
    }

    // Print a success message with the port number
    printf("Server started. Listening on port %d...\n", port);

    // Return the server socket descriptor
    return server_socket;
}



/**
 * Function to handle incoming client connections.
 *
 * @param   server_socket   An integer representing the server socket descriptor.
 */
void handle_clients(int server_socket) {
    // Infinite loop to continuously listen for and handle client connections
    while (1) {
        // Check if the number of currently connected clients is less than the maximum allowed
        if (num_clients < MAX_CLIENTS) {
            // Declare a variable for the client's address and its length
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);

            // Accept a new client connection
            int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket < 0) {
                // Print an error message and continue to the next iteration if the accept call failed
                perror("Error accepting connection");
                continue;
            }

            // Print a message indicating that a new client has connected
            printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add the new client socket to the client_sockets array and increment the client count
            client_sockets[num_clients] = client_socket;
            num_clients++;

            // Declare a variable for the thread ID
            pthread_t tid;

            // Create a new thread to handle the client
            int ret = pthread_create(&tid, NULL, client_handler, &client_socket);
            if (ret < 0) {
                // Print an error message, decrement the client count, and close the client socket if the thread creation failed
                perror("Error creating thread");
                num_clients--;
                close(client_socket);
            } else {
                // Print a message indicating that the client thread has started
                printf("Client thread started");

                // Detach the thread so that it can run independently
                pthread_detach(tid);
            }
        } else {
            // Print a message indicating that the maximum number of clients has been reached
            printf("Max number of clients reached. Waiting for a client to disconnect...\n");

            // Loop over the client_sockets array
            for (int i = 0; i < num_clients; i++) {
                // Skip to the next iteration if the client socket is -1
                if (client_sockets[i] == -1) {
                    continue;
                }

                // Send a zero-length message to the client to check if it's still connected
                if (send(client_sockets[i], NULL, 0, MSG_NOSIGNAL) < 0) {
                    // If the send call failed, close the client socket, set it to -1 in the client_sockets array, and decrement the client count
                    close(client_sockets[i]);
                    client_sockets[i] = -1;
                    num_clients--;
                }
            }
        }
    }
}


/**
 * Entry point for the thread handler.
 *
 * @param   argc    The number of command-line arguments.
 * @param   argv    The command-line arguments.
 * @return  An integer representing the status of the function execution. 0 for successful execution, non-zero for error.
 */
int thread_handler() {
    // Set up a server socket on the specified port
    int server_socket = setup_server_socket(PORT);
    // If the socket setup failed (indicated by a negative return value), return 1 to indicate an error
    if (server_socket < 0) {
        return 1;
    }

    // If the socket setup was successful, handle incoming client connections on the server socket
    handle_clients(server_socket);

    // Return 0 to indicate successful execution of the function
    return 0;
}

int num_clients = 0;
int client_sockets[MAX_CLIENTS];


int run_server()
{
    signal(SIGINT, signal_handler);
    thread_handler();
}

int main()
{
    run_server();
}