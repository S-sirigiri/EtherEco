#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <cstring>
#include <thread>
#include <string>
#include <sstream>


#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 1024




///////////////////////
//ClASS DECLARATIONS//
/////////////////////

/*
 * Class: SendReceive
 * Encapsulates methods and attributes to handle socket programing
 */
class SendReceive
{
protected:
    int sock;
    struct sockaddr_in server;


public:
    SendReceive(char *server_ip, int port);
    ~SendReceive();
    int receiveDataOnce(char *data);
    int sendDataOnce(char* buffer);
};

/*
 * Class: ClientTransaction
 * Encapsulates methods and attributes to facilitate all the client transactions
 */
class ClientTransaction : public SendReceive
{
private:
    char* rcptUsername;
    char* username;
    char* password;

    void receiveData();
    void sendData();

public:
    ClientTransaction(char *server_ip, int port,
                      char* username, char* password, char* rcptUsername);
    bool handshakeTransaction();
    bool fetchBufferMessages();
    int threadHandler();
    void updateCredentials(char* username, char* password, char* rcptUsername);
};

#endif /* CLIENT_H */