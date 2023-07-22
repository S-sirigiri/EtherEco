#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>

#define MAX_QUERY_SIZE 1024

///////////////////////
//FUNCTION DECLARATIONS
///////////////////////


void create_tables();
void add_new_user(char *username, char *password);

int find_username_from_database(char *key, char *username);
int authenticate_user_from_database(char *username, char *password);
PGresult *get_buffer_messages(char *client_username, char *rcpt_username);
void add_buffer_message(char *client_username, char *rcpt_username, char *message);



#endif /* DATABASE_H */