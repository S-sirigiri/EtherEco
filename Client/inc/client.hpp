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


class ClientTransaction : public SendReceive
{
private:
    char* rcptUsername;
    char* username;
    char* password;

protected:
    void receiveData();
    void sendData();

public:
    ClientTransaction(char *server_ip, int port,
                      char* username, char* password, char* rcptUsername);
    bool handshakeTransaction();
    bool fetchBufferMessages();
    int threadHandler();
};

#endif /* CLIENT_H */