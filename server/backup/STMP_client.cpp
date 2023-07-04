#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

// Function to send an email via SMTP
void send_email(std::string server_address, int port_number, std::string from_email_address, std::string to_email_address, std::string email_body) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(server_address.c_str());
    if (server == NULL) {
        fprintf(stderr, "Error: no such host as %s\n", server_address.c_str());
        exit(1);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    }
    char buffer[MAX_BUFFER_SIZE];
    int n = 0;
    buffer[n] = '\0';
    std::cout << buffer << std::endl;

    std::stringstream hello_message;
    hello_message << "HELO " << server_address << "\r\n";
    if (send(sockfd, hello_message.str().c_str(), strlen(hello_message.str().c_str()), 0) < 0) {
        perror("Error sending message to server");
        exit(1);
    }

    n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from server");
        exit(1);
    }
    buffer[n] = '\0';
    std::cout << buffer << std::endl;

    std::stringstream mail_from_command;
    mail_from_command << "MAIL FROM: <" << from_email_address << ">\r\n";
    if (send(sockfd, mail_from_command.str().c_str(), strlen(mail_from_command.str().c_str()), 0) < 0) {
        perror("Error sending message to server");
        exit(1);
    }

    n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from server");
        exit(1);
    }
    buffer[n] = '\0';
    std::cout << buffer << std::endl;

    std::stringstream rcpt_to_command;
    rcpt_to_command << "RCPT TO: <" << to_email_address << ">\r\n";
    if (send(sockfd, rcpt_to_command.str().c_str(), strlen(rcpt_to_command.str().c_str()), 0) < 0) {
        perror("Error sending message to server");
        exit(1);
    }

    n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from server");
        exit(1);
    }
    buffer[n] = '\0';
    std::cout << buffer << std::endl;
    if (send(sockfd, "DATA\r\n", strlen("DATA\r\n"), 0) < 0) {
    	perror("Error sending message to server");
    	exit(1);
	}

	n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
	if (n < 0) {
    	perror("Error receiving message from server");
    	exit(1);
	}
	buffer[n] = '\0';
	std::cout << buffer << std::endl;

    //debug
    std::cout << "trying to send subject" << std::endl;
	std::stringstream email_subject;
	email_subject << "Subject: Test email\r\n";
	if (send(sockfd, email_subject.str().c_str(), strlen(email_subject.str().c_str()), 0) < 0) {
    	perror("Error sending message to server");
    	exit(1);
	}

    //debug
    std::cout << "Sent subject" << std::endl;
	std::stringstream email_content;
	email_content << email_body << "\r\n";
	if (send(sockfd, email_content.str().c_str(), strlen(email_content.str().c_str()), 0) < 0) {
    	perror("Error sending message to server");
    	exit(1);
	}

	if (send(sockfd, ".\r\n", strlen(".\r\n"), 0) < 0) {
    	perror("Error sending message to server");
    	exit(1);
	}

	n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
	if (n < 0) {
    	perror("Error receiving message from server");
    	exit(1);
	}
	buffer[n] = '\0';
	std::cout << buffer << std::endl;

	if (send(sockfd, "QUIT\r\n", strlen("QUIT\r\n"), 0) < 0) {
    	perror("Error sending message to server");
    	exit(1);
	}

	n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
	if (n < 0) {
    	perror("Error receiving message from server");
    	exit(1);
	}
	buffer[n] = '\0';
	std::cout << buffer << std::endl;

	close(sockfd);
}

int main() {
	std::string server_address = "localhost";
	int port_number = 12345;
	std::string from_email_address = "sree.sirigri@gmail.com";
	std::string to_email_address = "sree.sirigri@gmail.com";
	std::string email_body = "This is a test email sent via SMTP.";
	send_email(server_address, port_number, from_email_address, to_email_address, email_body);

	return 0;
}
