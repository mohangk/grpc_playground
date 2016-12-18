#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "article.grpc.pb.h"

#include "db.h"
#include "db/articles_query.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using publishing::Article;
using publishing::Articles;
using publishing::ArticleStore;

using publishing::ArticleRequest;
using publishing::ArticlesForPeriodRequest;

PGresult* queryArticles(int category_id, std::string start_timestamp, std::string end_timestamp)
{
  PGconn *conn;
  PGresult   *res;

  openConn("dbname=woi_production", &conn);
  std::string q("select id, title, short_desc, content_body from articles where publish_date >='" + start_timestamp + "' and publish_date <='" + end_timestamp +"' ");

  res = PQexec(conn, q.c_str());
  //res = exec(conn, q.c_str(),  PGRES_TUPLES_OK);
  PQfinish(conn);

  return res;
}

void populateArticleFromRS(Article* a, PGresult *res, int i)
{
    int id = std::stoi(PQgetvalue(res, i, 0));
    std::string title(PQgetvalue(res, i, 1));
    std::string short_desc(PQgetvalue(res, i, 2));
    std::string content_body(PQgetvalue(res, i, 3));

    a->set_id(id);
    a->set_title(title);
    a->set_short_desc(short_desc);
    a->set_content_body(content_body);
}

class ArticleStoreServiceImpl final : public ArticleStore::Service {

  Status GetArticle(ServerContext* context, const ArticleRequest* articleRequest, Article* article) override {

    auto q = ArticlesQuery();
    q.find(999, article);
    return Status::OK;
  }


  Status ArticlesForPeriod(ServerContext* ctx,
      const ArticlesForPeriodRequest* r,
      grpc::ServerWriter<Article>* w) override {

    ctx->set_compression_algorithm(GRPC_COMPRESS_GZIP);

    PGresult *res = queryArticles(r->category_id(), r->start_timestamp(), r->end_timestamp());

    for(int i=0; i < PQntuples(res); i++) {
      Article a = Article();
      populateArticleFromRS(&a, res, i);
      w->Write(a);
    }

    PQclear(res);

    return Status::OK;
  }

  Status ArticlesForPeriodBulk(ServerContext* ctx,
      const ArticlesForPeriodRequest* r,
      Articles* articles) override {

    ctx->set_compression_algorithm(GRPC_COMPRESS_GZIP);

    PGresult *res = queryArticles(r->category_id(), r->start_timestamp(), r->end_timestamp());

    for(int i=0; i < PQntuples(res); i++) {
      Article* a = articles->add_article();
      populateArticleFromRS(a, res, i);
    }

    PQclear(res);
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

  RunServer();
  return 0;
};
