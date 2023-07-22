#include <database.h>

/**
 * Creates the necessary tables in the PostgreSQL database.
 *
 * @return None
 */
void create_tables()
{
    // Establish a connection to the PostgreSQL database
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    // Define the query to create the tables
    char *query = "CREATE TABLE IF NOT EXISTS users ( "
                  "    username VARCHAR(50) PRIMARY KEY, "
                  "    password VARCHAR(1000) NOT NULL"
                  ");"
                  "CREATE TABLE IF NOT EXISTS buffer_messages ( "
                  "    username VARCHAR(50) REFERENCES users(username), "
                  "    rcpt_username VARCHAR(50), "
                  "    message VARCHAR(2048) "
                  ");";
    // Execute the query
    PGresult* res = PQexec(conn, query);

    // Check the result of the query execution
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
    }

    // Clean up the result and close the database connection
    PQclear(res);
    PQfinish(conn);

    return;
}


/**
 * Adds a new user to the PostgreSQL database.
 *
 * @param username - The username of the new user.
 * @param password - The password of the new user.
 * @return None
 */
void add_new_user(char *username, char *password)
{
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    char query[MAX_QUERY_SIZE];
    sprintf(query, "INSERT INTO users "
                   "VALUES (%s, %s); ", username, password);
    PGresult* res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
    }

    PQclear(res);
    PQfinish(conn);

    return;
}

/**
 * Finds a username from the database based on a specified key.
 *
 * @param key - The key to search for in the database.
 * @param username - The variable to store the found username.
 * @return 0 if the username is found, 1 if not found, -1 if an error occurs.
 */
int find_username_from_database(char *key, char *username)
{
    // Establish a connection to the PostgreSQL database
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    // Prepare the query to insert a new user into the 'users' table
    char query[MAX_QUERY_SIZE];
    sprintf(query, "SELECT username "
                   "FROM users "
                   "WHERE username = '%s' ", username);

    // Execute the query
    PGresult *res = PQexec(conn, query);

    // Check the result of the query execution
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

    // Clean up the result and close the database connection
    if (PQntuples(res) > 0){
        PQclear(res);
        PQfinish(conn);
        return 0;
    } else {
        PQclear(res);
        PQfinish(conn);
        return 1;
    }
}


/**
 * Authenticates a user by checking their credentials against the database.
 *
 * @param username - The username of the user to authenticate.
 * @param password - The password of the user to authenticate.
 * @return 0 if the user is authenticated successfully,
 *         1 if the user authentication fails,
 *        -1 if there is an error in the authentication process.
 */
int authenticate_user_from_database(char *username, char *password)
{
    // Establish a connection to the PostgreSQL database
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    // Prepare the query to fetch the password from the 'users' table for the given username
    char query[MAX_QUERY_SIZE];
    sprintf(query, "SELECT password "
                   "FROM users "
                   "WHERE username = '%s' ", username);
    // Execute the query
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

    // Compare the fetched password with the provided password
    if ( strcmp(PQgetvalue(res, 0, 0), password) == 0 ) {
        PQclear(res);
        PQfinish(conn);
        return 0;
    } else {
        PQclear(res);
        PQfinish(conn);
        return 1;
    }
}


/**
 * Retrieves buffer messages for a client from the database and deletes them afterwards.
 *
 * @param client_username - The username of the client to retrieve buffer messages for.
 * @param rcpt_username - The username of the recipient to filter buffer messages.
 * @return PGresult pointer to the result set containing buffer messages,
 *         or NULL if there is an error in the retrieval process.
 */
PGresult *get_buffer_messages(char *client_username, char *rcpt_username)
{
    // Establish a connection to the PostgreSQL database
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    // Prepare the query to fetch buffer messages for the client
    char query[MAX_QUERY_SIZE];
    sprintf(query, "SELECT message "
                   "FROM buffer_messages "
                   "WHERE rcpt_username = '%s' "
                   "ORDER BY username ", client_username);

    // Execute the query
    PGresult *res = PQexec(conn, query);

    // Check the result of the query execution
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    // Prepare the query to delete buffer messages for the client
    sprintf(query, "DELETE FROM buffer_messages "
                   "WHERE rcpt_username = '%s' ", client_username);

    // Execute the delete query
    PGresult *res2 = PQexec(conn, query);

    // Check the result of the delete query execution
    if (PQresultStatus(res2) != PGRES_COMMAND_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        return NULL;
    }

    // Return the result set containing buffer messages
    return res;
}


/**
 * Adds a buffer message to the database for a specific client and recipient.
 *
 * @param client_username - The username of the client sending the buffer message.
 * @param rcpt_username - The username of the recipient of the buffer message.
 * @param message - The buffer message to be added.
 */
void add_buffer_message(char *client_username, char *rcpt_username, char *message)
{
    // Establish a connection to the PostgreSQL database
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    // Prepare the query to insert the buffer message
    char query[MAX_QUERY_SIZE];
    sprintf(query,"INSERT INTO buffer_messages "
                   "VALUES ('%s', '%s', '%s') ", client_username, rcpt_username, message);

    // Execute the query
    PGresult *res2 = PQexec(conn, query);

    // Check the result of the query execution
    if (PQresultStatus(res2) != PGRES_COMMAND_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
    }

    return;
}

