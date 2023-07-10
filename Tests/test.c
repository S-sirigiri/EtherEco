#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <string.h>

#define MAX_QUERY_SIZE 1024

int main(int argc, char **argv)
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
                   "WHERE username = '%s'", "Admin");
    PGresult *res = PQexec(conn, query);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        printf("Query execution failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 1;
    } else {
        printf("All good\n");
    }
}
