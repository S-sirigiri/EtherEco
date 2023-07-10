#include <database.h>

void create_tables()
{
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    char *query = "CREATE TABLE IF NOT EXISTS users ( "
                  "    username VARCHAR(50) PRIMARY KEY, "
                  "    password VARCHAR(1000) NOT NULL"
                  ");"
                  "CREATE TABLE IF NOT EXISTS buffer_messages ( "
                  "    username VARCHAR(50) REFERENCES users(username), "
                  "    rcpt_username VARCHAR(50), "
                  "    message VARCHAR(2048) "
                  ");";
    PGresult* res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
    }

    PQclear(res);
    PQfinish(conn);

    return;
}

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

int find_username_from_database(char *key, char *username)
{
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    char query[MAX_QUERY_SIZE];
    sprintf(query, "SELECT username "
                   "FROM users "
                   "WHERE username = '%s' ", username);
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

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

int authenticate_user_from_database(char *username, char *password)
{
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return -1;
    }

    char query[MAX_QUERY_SIZE];
    sprintf(query, "SELECT password "
                   "FROM users "
                   "WHERE username = '%s' ", username);
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return -1;
    }

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

PGresult *get_buffer_messages(char *client_username, char *rcpt_username)
{
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }

    char query[MAX_QUERY_SIZE];
    sprintf(query, "SELECT messages "
                   "FROM buffer_messages "
                   "WHERE username = '%s' "
                   "ORDER BY rcpt_username ", client_username);
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return NULL;
    }

    sprintf(query, "DELETE FROM buffer_messages "
                   "WHERE username = '%s' ", client_username);
    PGresult *res2 = PQexec(conn, query);

    if (PQresultStatus(res2) != PGRES_COMMAND_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        return NULL;
    }

    return res;
}

void add_buffer_message(char *client_username, char *rcpt_username, char *message)
{
    PGconn *conn = PQconnectdb("host=localhost port=5432 dbname=postgres user=postgres password=postgres");

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return;
    }

    char query[MAX_QUERY_SIZE];
    sprintf(query,"INSERT INTO buffer_messages "
                   "VALUES ('%s', '%s', '%s') ", client_username, rcpt_username, message);
    PGresult *res2 = PQexec(conn, query);

    if (PQresultStatus(res2) != PGRES_COMMAND_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
    }

    return;
}

