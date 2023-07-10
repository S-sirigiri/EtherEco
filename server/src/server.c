#include <server.h>


void signal_handler(int signalNumber) {
    if (signalNumber == SIGINT) {
        printf("\nHalting the server....\n");
        exit(0);
    }
}


int transact_recv_send_map_reduce(int client_socket, int (*map_reduce_function)(char*, char*), char *recv_str, char *send_str, char* error)
{
    char recv_buffer[BUFFER_SIZE];

    int bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE, 0);

    if (bytes_received > 0)
    {
        recv_buffer[bytes_received] = '\0';

        if ((*map_reduce_function)(recv_str, recv_buffer) != 0)
        {
            char send_buffer[BUFFER_SIZE];
            sprintf(send_buffer, "%s <%s>", error, recv_buffer);
            send(client_socket, send_buffer, strlen(send_buffer), 0);
            return (-1 * bytes_received);
        }
        else
        {
            strcpy(recv_str, recv_buffer);
            char send_buffer[BUFFER_SIZE];
            strcpy(send_buffer, send_str);
            send(client_socket, send_buffer, strlen(send_buffer), 0);
            return bytes_received;
        }

    }
    else
    {
        char *send_buffer = "000";
        send(client_socket, send_buffer, strlen(send_buffer), 0);
        return 0;
    }
}

int transact_send_recv_map_reduce(int client_socket, int (*map_reduce_function)(char*, char*), char *recv_str, char *send_str, char* error)
{
    send(client_socket, send_str, strlen(send_str), 0);

    char recv_buffer[BUFFER_SIZE];

    int bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE, 0);

    if (bytes_received > 0)
    {
        recv_buffer[bytes_received] = '\0';

        if ((*map_reduce_function)(recv_str, recv_buffer) != 0)
        {
            char send_buffer[BUFFER_SIZE];
            sprintf(send_buffer, "%s <%s>", error, recv_buffer);
            send(client_socket, send_buffer, strlen(send_buffer), 0);
            return (-1 * bytes_received);
        }
        else
        {
            return bytes_received;
        }

    }
    else
    {
        char *send_buffer = "000";
        send(client_socket, send_buffer, strlen(send_buffer), 0);
        return 0;
    }

}

int strcmp_wrapper(char* arg1, char* arg2) {return strcmp(arg1, arg2);}


void *client_handler(void *arg) {
    int client_socket = *((int *)arg);
	char client_username[BUFFER_SIZE] = "user";
	char rcpt_username[BUFFER_SIZE] = "user";

    char HELO[] = "HELO";
    char WHO[] = "WHO";
    char AUTH[] = "AUTH";
    char TO[] = "TO";
    char DATA[] = "DATA";
    char OK[] = "OK";
    char DOT[] = ".";


	//receiving HELO and sending WHO
    while ( transact_recv_send_map_reduce(client_socket, strcmp_wrapper, HELO, WHO, "403") <= 0 );
    //receiving client username and sending AUTH
    while ( transact_recv_send_map_reduce(client_socket, find_username_from_database, client_username, AUTH, "101") <= 0 );
    //receiving password and sending TO
    while ( transact_recv_send_map_reduce(client_socket, authenticate_user_from_database, client_username, TO, "102") <= 0 );
    //receiving recipient username and sending DATA
    while ( transact_recv_send_map_reduce(client_socket, find_username_from_database, rcpt_username, DATA, "101") <= 0 );

    //send the buffer messages
    PGresult *buffer_messages = get_buffer_messages(client_username, rcpt_username);

    char str_num_messages[BUFFER_SIZE];
    int num_messages = PQntuples(buffer_messages);
    sprintf(str_num_messages, "%d", num_messages);

    while ( transact_send_recv_map_reduce(client_socket, strcmp_wrapper, OK, str_num_messages, "202") <= 0 );

    for (int iter=0; iter < num_messages; iter++)
    {
        while ( transact_send_recv_map_reduce(client_socket, strcmp_wrapper, OK, PQgetvalue(buffer_messages, iter, 0), "202") <= 0 );
    }

    //set the user as online
    set_online(client_username, client_socket);

    //client to client transaction
    while(1)
    {
        //check if recipient is online
        int rcpt_socket = is_online(rcpt_username);

        char recv_buffer[BUFFER_SIZE];
        //get the client message
        int bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE, 0);

        if (bytes_received > 0)
        {
            recv_buffer[bytes_received] = '\0';
            //disconnect command
            if (strcmp(recv_buffer, ".") == 0)
            {
                //user disconnected
                set_offline(client_username);
                break;
            }
            //send or store the message based if the recipient is online or not
            if (rcpt_socket > 0)
            {
                while (transact_send_recv_map_reduce(rcpt_socket, strcmp_wrapper, "OK", recv_buffer, "202") <= 0);
            }
            else
            {
                add_buffer_message(client_username, rcpt_username, recv_buffer);
            }
        }
        else
        {
            //user disconnected
            set_offline(client_username);
            break;
        }
    }

    // Close the socket and remove it from the client_sockets array
    close(client_socket);
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = -1;
            break;
        }
    }
    return (void*) 1;
}

int main(int argc, char *argv[]) {

    signal(SIGINT, signal_handler);

    int port = 12345;
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
