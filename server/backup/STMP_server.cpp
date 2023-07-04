#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

// Function to handle incoming email message
void handle_email(int client_sockfd) {
    char buffer[MAX_BUFFER_SIZE];
    //debug
    std::cout << "RECV" <<std::endl;
    int n = 0;

    buffer[n] = '\0';
    std::cout << buffer << std::endl;
    std::stringstream response_message;
    response_message << "220 Ready to receive email\r\n";
    if (send(client_sockfd, response_message.str().c_str(), strlen(response_message.str().c_str()), 0) < 0) {
        perror("Error sending message to client");
        exit(1);
    }

    n = recv(client_sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from client");
        exit(1);
    }
    buffer[n] = '\0';
    std::cout << buffer << std::endl;

    response_message.str("");
    response_message << "250 OK\r\n";
    if (send(client_sockfd, response_message.str().c_str(), strlen(response_message.str().c_str()), 0) < 0) {
        perror("Error sending message to client");
        exit(1);
    }

    n = recv(client_sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from client");
        exit(1);
    }
    buffer[n] = '\0';
    std::cout << buffer << std::endl;

    response_message.str("");
    response_message << "250 OK\r\n";
    if (send(client_sockfd, response_message.str().c_str(), strlen(response_message.str().c_str()), 0) < 0) {
        perror("Error sending message to client");
        exit(1);
    }

    n = recv(client_sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from client");
        exit(1);
    }
    buffer[n] = '\0';
    std::cout << "Here" << std::endl;
    std::cout << buffer << std::endl;

    response_message.str("");
    response_message << "250 OK\r\n";
    if (send(client_sockfd, response_message.str().c_str(), strlen(response_message.str().c_str()), 0) < 0) {
        perror("Error sending message to client");
        exit(1);
    }
    close(client_sockfd);
}

int main(int argc, char *argv[]) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    int port_number = 12345;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket to address");
        exit(1);
    }

    if (listen(sockfd, 5)< 0) {
	    perror("Error listening for incoming connections");
		exit(1);
	}

	std::cout << "Server listening on port " << port_number << std::endl;

	while (true) {
    	struct sockaddr_in client_addr;
    	socklen_t client_addrlen = sizeof(client_addr);
    	int client_sockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_addrlen);
    	if (client_sockfd < 0) {
        	perror("Error accepting incoming connection");
        	exit(1);
    	}

    		std::cout << "Incoming connection accepted" << std::endl;

    		handle_email(client_sockfd);
	}

	close(sockfd);
	return 0;
}

