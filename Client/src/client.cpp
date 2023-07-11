#include "client.hpp"
#include "fileIO.hpp"

/*
 * Class constructor
 * Creates the socket and connects to the server
 */
SendReceive::SendReceive(char *server_ip, int port)
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
SendReceive::~SendReceive()
{
    close(this->sock);
}

/*
 * Method to handle receiving data
 * Arguments: string to store the received string
 * Returns: the number of bytes received
 */
int SendReceive::receiveDataOnce(char *data)
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
int SendReceive::sendDataOnce(char* buffer)
{
    int sent_bytes = send(this->sock, buffer, strlen(buffer), 0);
    if (sent_bytes < 0)
    {
        std::cerr << "Failed to send the message" << std::endl;
        exit(-1);
    }
    return sent_bytes;
}



/*
 * Class constructor
 */
ClientTransaction::ClientTransaction(char* server_ip, int port,
                                     char* username, char* password, char* rcptUsername) : SendReceive(server_ip, port)
{
    //create the directory where all the chats are stored
    FileIO::createDirectoryIfNotExists("chats");
    strcpy(this->username, username);
    strcpy(this->rcptUsername, rcptUsername);
    strcpy(this->password, password);
}

/* Method: receiveData
 * Thread function to handle receiving data
 * Arguments: None
 * Returns: None
 */
void ClientTransaction::receiveData()
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

        char sentUsername[50];
        sscanf(buffer, "<%[^>]>: %*s", sentUsername);
        if (!strcmp(sentUsername,this->rcptUsername))
        {
            std::cout << buffer << std::endl;
            FileIO::writeToFile(sentUsername, buffer);
        }
        else
        {
            FileIO::writeToFile(sentUsername, buffer);
        }
    }
}

/* Method: sendData
 * Thread function to handle sending data
 * Arguments: None
 * Returns: None
 */
void ClientTransaction::sendData()
{
    char buffer[BUFLEN];
    char input[BUFLEN-54];
    while(true)
    {
        std::cout << "<" << this->username << ">: ";
        std::cin.getline(input, BUFLEN-54);
        sprintf(buffer, "<%s>: %s", this->username, input);
        if (send(this->sock, buffer, strlen(buffer), 0) < 0)
        {
            std::cerr << "Failed to send the message" << std::endl;
            break;
        }
    }
}


/*
 * Method to perform the handshake
 * Arguments: None
 * Returns: true if handshake was successful
 */
bool ClientTransaction::handshakeTransaction()
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
    if(strcmp(buffer, "DATA"))
    {
        std::cerr << "Failed to receive valid command from server" << std::endl;
        return false;
    }

    return true;

}

/*
 * Method to get all the buffer messages
 * Arguments: None
 * Returns: void
 */
bool ClientTransaction::fetchBufferMessages()
{
    int bytesReceived = 0;

    char OK[3] = "OK";
    char buffer[BUFLEN];
    memset(buffer, '\0', BUFLEN);

    bytesReceived = this->receiveDataOnce(buffer);
    buffer[bytesReceived] = '\0';
    this->sendDataOnce(OK);
    std::istringstream iss(buffer);
    int numMessages = 0;
    iss >> numMessages;


    for (int iter=0; iter < numMessages; iter++)
    {
        bytesReceived = receiveDataOnce(buffer);
        buffer[bytesReceived] = '\0';

        char rcptfilename[50];
        sscanf(buffer, "<%[^>]>: %*s", rcptfilename);

        std::string bufferMessages = "";
        bufferMessages += buffer;
        bufferMessages += '\n';

        FileIO::writeToFile(rcptfilename, bufferMessages);

        this->sendDataOnce(OK);
    }
    return true;
}

int ClientTransaction::threadHandler()
{
    FileIO::readFile(this->rcptUsername);

    // Create send and receive threads
    std::thread send_thread(&ClientTransaction::sendData, this);
    std::thread receive_thread(&ClientTransaction::receiveData, this);

    // Wait for both threads to finish
    send_thread.join();
    receive_thread.join();

    return 0;
}


void ClientTransaction::updateCredentials(char* username, char* password, char* rcptUsername)
{
    strcpy(this->username, username);
    strcpy(this->password, password);
    strcpy(this->rcptUsername, rcptUsername);
}