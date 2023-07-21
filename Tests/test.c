//not an actual test ingore these files

int thread_handler(int argc, char *argv[]) {

    int port = 12346;
    // Create a socket and bind it to the specified port
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error creating socket");
        return 1;
    }
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        return 1;
    }

    // Listen for incoming connections on the socket
    if (listen(server_socket, 5) < 0) {
        perror("Error listening for connections");
        return 1;
    }
    printf("Server started. Listening on port %d...\n", port);

    // Accept incoming connections and spawn threads to handle clients
    while (1) {
        if (num_clients < MAX_CLIENTS) {
            // Accept a new connection
            struct sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
            if (client_socket < 0) {
                perror("Error accepting connection");
                continue;
            }
            printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            // Add the new client_socket to the client_sockets array and spawn a new thread to handle the client
            client_sockets[num_clients] = client_socket;
            num_clients++;
            pthread_t tid;
            int ret = pthread_create(&tid, NULL, client_handler, &client_socket);
            if (ret < 0) {
                perror("Error creating thread");
                num_clients--;
                close(client_socket);
            } else {
                // Detach the thread so it can run independently
                printf("Client thread started");
                pthread_detach(tid);
            }
        } else {
            // Max number of clients reached, wait for a client to disconnect
            printf("Max number of clients reached. Waiting for a client to disconnect...\n");
            for (int i = 0; i < num_clients; i++) {
                if (client_sockets[i] == -1) {
                    continue;
                }
                // Check if the client is still connected
                if (send(client_sockets[i], NULL, 0, MSG_NOSIGNAL) < 0) {
                    // Client disconnected, remove it from the client_sockets array
                    close(client_sockets[i]);
                    client_sockets[i] = -1;
                    num_clients--;
                }
            }
        }
    }
}



int interprocess_shared_memory_receive(const char *name, char *message)
{
    int shm_fd;
    void *ptr;
/* open the shared memory segment */

    shm_fd = shm_open(name, O_RDONLY, 0666);
    if (shm_fd == -1)
    {
        printf("Shared memory failed\n");
        return -1;
    }

    */
/* map the shared memory segment to the address space of the process */

    ptr = mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        printf("Map failed\n");
        return -1;
    }

    */
/* read from the shared memory segment */

    sscanf((char *)ptr, "%s", message);

    */
/* remove the shared memory segment */

    if (shm_unlink(name) == -1)
    {
        printf("Error removing %s\n", name);
        return -1;
    }

    return 0;
}
