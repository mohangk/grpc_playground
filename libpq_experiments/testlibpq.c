#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}


void open_connection(const char *conninfo, PGconn **conn)
{
    *conn = PQconnectdb(conninfo);

    if (PQstatus(*conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(*conn));
        exit_nicely(*conn);
    }
}


void begin(PGconn **conn)
{
    PGresult   *res;
    res = PQexec(*conn, "BEGIN");
    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "BEGIN command failed: %s", PQerrorMessage(*conn));
        PQclear(res);
        exit_nicely(*conn);
    }
    PQclear(res);
}


PGresult *exec(PGconn **conn, char* stmt, int expectedStatus)
{
    PGresult   *res;
    res = PQexec(*conn, stmt);
    if (PQresultStatus(res) != expectedStatus)
    {
        fprintf(stderr, "failed: %s", PQerrorMessage(*conn));
        PQclear(res);
        exit_nicely(*conn);
    }
    return res;
}

void printResults(PGresult *res)
{
    /* first, print out the attribute names */
    int nFields = PQnfields(res);
    for (int i = 0; i < nFields; i++)
        printf("%-15s", PQfname(res, i));
    printf("\n\n");

    /* next, print out the rows */
    for (int i = 0; i < PQntuples(res); i++)
    {
        for (int j = 0; j < nFields; j++)
            printf("%-15s", PQgetvalue(res, i, j));
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    const char *conninfo;
    PGconn     *conn;
    PGresult   *res;
    if (argc > 1)
        conninfo = argv[1];
    else
        conninfo = "dbname = postgres";

    open_connection(conninfo, &conn);
    begin(&conn);


    res = exec(&conn, "DECLARE myportal CURSOR FOR select * from pg_database", PGRES_COMMAND_OK);
    PQclear(res);

    res = exec(&conn, "FETCH ALL in myportal",  PGRES_TUPLES_OK);

    printResults(res);

    PQclear(res);

    res = PQexec(conn, "CLOSE myportal");
    PQclear(res);

    res = PQexec(conn, "END");
    PQclear(res);

    PQfinish(conn);

    return 0;
}
