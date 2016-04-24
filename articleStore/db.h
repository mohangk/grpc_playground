#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

static void
exit_nicely(PGconn *conn)
{
    PQfinish(conn);
    exit(1);
}

void openConn(const char *conninfo, PGconn **conn)
{
    *conn = PQconnectdb(conninfo);

    if (PQstatus(*conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s",
                PQerrorMessage(*conn));
        exit_nicely(*conn);
    }
}

PGresult *exec(PGconn *conn, const char* stmt, int expectedStatus)
{
    PGresult   *res;
    res = PQexec(conn, stmt);
    if (PQresultStatus(res) != expectedStatus)
    {
        fprintf(stderr, "failed: %s", PQerrorMessage(conn));
        PQclear(res);
        exit_nicely(conn);
    }
    return res;
}

