#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 1024

void send_email(char* server_address, int port_number, char* from_email_address, char* to_email_address, char* email_body) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    struct hostent *server = gethostbyname(server_address);
    if (server == NULL) {
        fprintf(stderr, "Error: no such host as %s\n", server_address);
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
    printf("%s\n", buffer);

    char hello_message[MAX_BUFFER_SIZE];
    sprintf(hello_message, "HELO %s\r\n", server_address);
    if (send(sockfd, hello_message, strlen(hello_message), 0) < 0) {
        perror("Error sending message to server");
        exit(1);
    }

    n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from server");
        exit(1);
    }
    buffer[n] = '\0';
    
	if (strcmp(buffer, "WHO") != 0)
	{
		sprintf(buffer, "Server: %s", buffer);
		perror(buffer);
		exit(1);
	}

    char from_who_command[MAX_BUFFER_SIZE];
    sprintf(from_who_command, "%s", from_email_address);
    if (send(sockfd, from_who_command, strlen(from_who_command), 0) < 0) {
        perror("Error sending message to server");
        exit(1);
    }

    n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from server");
        exit(1);
    }
    buffer[n] = '\0';
    printf("%s\n", buffer);

	do
	{
		char password[50];
		printf("Enter password: ");
		scanf("%s", password);
    	char password_command[MAX_BUFFER_SIZE];
    	sprintf(password_command, "%s", password);
    	if (send(sockfd, from_who_command, strlen(password_command), 0) < 0) {
        	perror("Error sending message to server");
        	exit(1);
    	}

    	n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    	if (n < 0) {
        	perror("Error receiving message from server");
        	exit(1);
    	}
    	buffer[n] = '\0';
    	printf("%s\n", buffer);
	}while(strcmp(buffer, "AUTH"));

	if (strcmp(buffer, "RCPT") != 0)
	{
		sprintf(buffer, "Server: %s", buffer);
		perror(buffer);
		exit(1);
	}

    char rcpt_to_command[MAX_BUFFER_SIZE];
    sprintf(rcpt_to_command, "%s", to_email_address);
    if (send(sockfd, rcpt_to_command, strlen(rcpt_to_command), 0) < 0) {
        perror("Error sending message to server");
        exit(1);
    }

    n = recv(sockfd, buffer, MAX_BUFFER_SIZE - 1, 0);
    if (n < 0) {
        perror("Error receiving message from server");
        exit(1);
    }
    buffer[n] = '\0';
    printf("%s\n", buffer);

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
    printf("%s\n", buffer);

    printf("trying to send subject\n");
    char email_subject[MAX_BUFFER_SIZE];
    sprintf(email_subject, "Subject: Test email\r\n");
    if (send(sockfd, email_subject, strlen(email_subject), 0) < 0) {
        perror("Error sending message to server");
        exit(1);
    }

    printf("Sent subject\n");
    char email_content[MAX_BUFFER_SIZE];
    sprintf(email_content, "%s\r\n", email_body);
    if (send(sockfd, email_content, strlen(email_content), 0) < 0) {
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
    printf("%s\n", buffer);

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
    printf("%s\n", buffer);

    close(sockfd);
}

int main() {
    char* server_address = "localhost";
    int port_number = 12345;
    char* from_email_address = "fromusername";
    char* to_email_address = "tousername";
    char* email_body = "This is a test email sent via SMTP.";
    send_email(server_address, port_number, from_email_address, to_email_address, email_body);

    return 0;
}

