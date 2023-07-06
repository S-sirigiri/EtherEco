#include <iostream>
#include <cstring>
#include <thread>


#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define BUFLEN 1024


/*
 * Class: SendReceive
 * Encapsulates methods and attributes to handle socket programing
 */
class SendReceive
{
protected:
    //socket id
    int sock;
    //socket attributes
    struct sockaddr_in server;


public:
    /*
     * Class constructor
     * Creates the socket and connects to the server
     */
    SendReceive(char *server_ip, int port)
    {
        // Create socket
        this->sock = socket(AF_INET , SOCK_STREAM , 0);
        if (this->sock == -1)
        {
            std::cerr << "Could not create socket" << std::endl;
            throw std::runtime_error("Failed to create socket");
        }

        this->server.sin_addr.s_addr = inet_addr(server_ip);
        this->server.sin_family = AF_INET;
        this->server.sin_port = htons(port);

        // Connect to remote server
        if (connect(this->sock, (struct sockaddr *)&server , sizeof(this->server)) < 0)
        {
            std::cerr << "Connect failed" << std::endl;
            throw std::runtime_error("Failed to connect to server");
        }

        std::cout << "Connected to server" << std::endl;
    }

    /*
     * Class destroyer
     * Closes the socket
     */
    ~SendReceive()
    {
        close(this->sock);
    }

    /*
     * Method to handle receiving data
     * Arguments: string to store the received string
     * Returns: the number of bytes received
     */
    int receiveDataOnce(char *data)
    {

        char buffer[BUFLEN];
        memset(buffer, 0, BUFLEN);
        //receive data
        ssize_t received = recv(this->sock, buffer, BUFLEN - 1, 0);
        if (received <= 0)
        {
            std::cerr << "Failed to receive or connection closed by server" << std::endl;
            return received;
        }
        buffer[received] = '\0';
        strcpy(data, buffer);
        return received;
    }

    /*
     * Method to handle sending data
     * Arguments: the string to be sent
     * Returns: int -> the number of bytes sent
     */
    int sendDataOnce(char* buffer)
    {
        int sent_bytes = send(this->sock, buffer, strlen(buffer), 0);
        if (sent_bytes < 0)
        {
            std::cerr << "Failed to send the message" << std::endl;
            exit(-1);
        }
        return sent_bytes;
    }
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

protected:
    /* Method: receiveData
     * Thread function to handle receiving data
     * Arguments: None
     * Returns: None
     */
    void receiveData()
    {
        char buffer[BUFLEN];
        while(true)
        {
            memset(buffer, 0, BUFLEN);
            ssize_t received = recv(this->sock, buffer, BUFLEN - 1, 0);
            if (received <= 0)
            {
                std::cerr << "Failed to receive or connection closed by server" << std::endl;
                break;
            }
            buffer[received] = '\0';
            std::cout << "Data received: " << buffer << std::endl;
        }
    }

    /* Method: sendData
     * Thread function to handle sending data
     * Arguments: None
     * Returns: None
     */
    void sendData()
    {
        char buffer[BUFLEN];
        while(true)
        {
            std::cout << "Enter message to send: ";
            std::cin.getline(buffer, BUFLEN);
            if (send(this->sock, buffer, strlen(buffer), 0) < 0)
            {
                std::cerr << "Failed to send the message" << std::endl;
                break;
            }
        }
    }

public:

    /*
     * Class constructor
     */
    ClientTransaction(char *server_ip, int port,
                      char* username, char* password, char* rcptUsername) : SendReceive(server_ip, port)
    {
        //create the directory where all the chats are stored
        FileIO::createDirectoryIfNotExists("chats");
        strcpy(this->username, username);
        strcpy(this->rcptUsername, rcptUsername);
        strcpy(this->password, password);
        //read the previous chat
        FileIO::readFile(this->rcptUsername);
    }

    /*
     * Method to perform the handshake
     * Arguments: None
     * Returns: void
     */
    bool handshakeTransaction()
    {
        int bytesReceived = 0;

        char buffer[BUFLEN];
        memset(buffer, '\0', BUFLEN);
        strcpy(buffer, "HELO");
        this->sendDataOnce(buffer);

        memset(buffer, '\0', BUFLEN);
        bytesReceived = this->receiveDataOnce(buffer);
        buffer[bytesReceived] = '\0';
        if(strcmp(buffer, "WHO"))
        {
            std::cerr << "Failed to receive valid command from server" << std::endl;
            return false;
        }

        this->sendDataOnce(this->username);

        memset(buffer, '\0', BUFLEN);
        bytesReceived = this->receiveDataOnce(buffer);
        buffer[bytesReceived] = '\0';
        if(strcmp(buffer, "AUTH"))
        {
            std::cerr << "Failed to receive valid command from server" << std::endl;
            return false;
        }

        this->sendDataOnce(this->password);

        memset(buffer, '\0', BUFLEN);
        bytesReceived = this->receiveDataOnce(buffer);
        buffer[bytesReceived] = '\0';
        if(strcmp(buffer, "TO"))
        {
            std::cerr << "Failed to receive valid command from server" << std::endl;
            return false;
        }

        this->sendDataOnce(this->rcptUsername);

        memset(buffer, '\0', BUFLEN);
        bytesReceived = this->receiveDataOnce(buffer);
        buffer[bytesReceived] = '\0';
        if(strcmp(buffer, "TO"))
        {
            std::cerr << "Failed to receive valid command from server" << std::endl;
            return false;
        }


    }

    int threadHandler()
    {
        // Create send and receive threads
        std::thread send_thread(&ClientTransaction::sendData, this);
        std::thread receive_thread(&ClientTransaction::receiveData, this);

        // Wait for both threads to finish
        send_thread.join();
        receive_thread.join();

        return 0;
    }
};