#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "article.grpc.pb.h"

#include "articleRepository.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using publishing::Article;
using publishing::ArticleRequest;
using publishing::ArticleStore;

//TODO: split the database access to a separate class
PGconn     *conn;

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

//handle not found ?
void populateArticleById(int id, Article* article)
{

  PGresult   *res;

  std::string q("select title, short_desc, content_body from articles where id = " + std::to_string(id));
  res = exec(conn, q.c_str(),  PGRES_TUPLES_OK);
  std::string title(PQgetvalue(res, 0, 0));
  std::string short_desc(PQgetvalue(res, 0, 1));
  std::string content_body(PQgetvalue(res, 0, 2));

  article->set_id(id);
  article->set_title(title);
  article->set_short_desc(short_desc);
  article->set_content_body(content_body);
}

class ArticleStoreServiceImpl final : public ArticleStore::Service {
  Status find(ServerContext* context, const ArticleRequest* articleRequest, Article* article) override {

    populateArticleById(articleRequest->id(), article);
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  ArticleStoreServiceImpl service;

  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();

};

int main(int argc, char* argv[]) {
  openConn("dbname=woi_production", &conn);

  RunServer();
  return 0;
};
