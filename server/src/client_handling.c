#include "client_handling.h"



int transact_recv_send_map_reduce(int client_socket, int (*map_reduce_function)(char*, char*), char *recv_str, char *send_str, char* error)
{
    char recv_buffer[BUFFER_SIZE];

    int bytes_received = recv(client_socket, recv_buffer, BUFFER_SIZE, 0);

    if (bytes_received > 0)
    {
        recv_buffer[bytes_received] = '\0';

        if (!strcmp(recv_buffer, ".")){
            return -2;
        } else if (!strcmp(recv_buffer, "-")){
            return -3;
        }

        if ((*map_reduce_function)(recv_str, recv_buffer) != 0)
        {
            char send_buffer[BUFFER_SIZE];
            sprintf(send_buffer, "%s <%s>", error, recv_buffer);
            send(client_socket, send_buffer, strlen(send_buffer), 0);
            return -1;
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

        if (!strcmp(recv_buffer, ".")){
            return -2;
        } else if (!strcmp(recv_buffer, "-")){
            return -3;
        }

        if ((*map_reduce_function)(recv_str, recv_buffer) != 0)
        {
            char send_buffer[BUFFER_SIZE];
            sprintf(send_buffer, "%s <%s>", error, recv_buffer);
            send(client_socket, send_buffer, strlen(send_buffer), 0);
            return -1;
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


void close_connection(int client_socket){
    // Close the socket and remove it from the client_sockets array
    close(client_socket);
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] == client_socket) {
            client_sockets[i] = -1;
            break;
        }
    }
}

int transact_five_times(int (*func)(int, int (*)(char*, char*), char*, char*, char*), int client_socket, int (*map_reduce_function)(char*, char*), char *recv, char *send, char* error)
{
    int iter;
    for (iter =0; iter < 5; ++iter) {

        int result = func(client_socket, map_reduce_function, recv, send, error);

        if (result > 0){
            return 0;
        } else if (result == -2) {
            close_connection(client_socket);
            return -1;
        } else if (result == -3) {
            return 1;
        }
    }

    close_connection(client_socket);
    return -1;
}


void *client_handler(void *arg) {
    int client_socket = *((int *)arg);
    char client_username[BUFFER_SIZE] = "user";
    char rcpt_username[BUFFER_SIZE] = "user";
    char temp[BUFFER_SIZE] = "user";

    char HELO[] = "HELO";
    char WHO[] = "WHO";
    char AUTH[] = "AUTH";
    char TO[] = "TO";
    char DATA[] = "DATA";
    char OK[] = "OK";
    char DOT[] = ".";


    //receiving HELO and sending WHO
    if (transact_five_times(transact_recv_send_map_reduce, client_socket, strcmp_wrapper, HELO, WHO, "403") == -1 ) {
        return NULL;
    }
    //receiving client username and sending AUTH
    if( transact_five_times(transact_recv_send_map_reduce, client_socket, find_username_from_database, client_username, AUTH, "101") == -1 ) {
        return NULL;
    }
    //receiving password and sending TO
    strcpy(temp, client_username);
    if ( transact_five_times(transact_recv_send_map_reduce, client_socket, authenticate_user_from_database, client_username, TO, "102") == -1 ) {
        return NULL;
    }
    strcpy(client_username, temp);
    //receiving recipient username and sending DATA
    if (transact_five_times(transact_recv_send_map_reduce, client_socket, find_username_from_database, rcpt_username, DATA, "101") == -1) {
        return NULL;
    }

    //send the buffer messages
    PGresult *buffer_messages = get_buffer_messages(client_username, rcpt_username);

    char str_num_messages[BUFFER_SIZE];
    int num_messages = PQntuples(buffer_messages);
    sprintf(str_num_messages, "%d", num_messages);

    if (transact_five_times(transact_send_recv_map_reduce, client_socket, strcmp_wrapper, OK, str_num_messages, "202") == -1) {
        return NULL;
    }

    for (int iter=0; iter < num_messages; iter++)
    {
        while ( transact_send_recv_map_reduce(client_socket, strcmp_wrapper, OK, PQgetvalue(buffer_messages, iter, 0), "202") <= 0 );
    }

    //set the user as online
    set_online(client_username, client_socket);

    //client to client transaction
    while(1)
    {
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
            //check if recipient is online
            int rcpt_socket = is_online(rcpt_username);
            //send or store the message based if the recipient is online or not
            if (rcpt_socket > 0)
            {
                //while (transact_send_recv_map_reduce(rcpt_socket, strcmp_wrapper, OK, recv_buffer, "202") <= 0);
                send(rcpt_socket, recv_buffer, strlen(recv_buffer), 0);
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

    close_connection(client_socket);

    return NULL;
}