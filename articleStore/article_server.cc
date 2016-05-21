#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "article.grpc.pb.h"

#include "db.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using publishing::Article;
using publishing::Articles;
using publishing::ArticleStore;

using publishing::ArticleRequest;
using publishing::ArticlesForPeriodRequest;

//TODO: split the database access to a separate class
PGconn     *conn;

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
    populateArticleById(articleRequest->id(), article);
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

    return Status::OK;
  }

  PGresult* queryArticles(int category_id, std::string start_timestamp, std::string end_timestamp)
  {
    PGresult   *res;

    std::string q("select id, title, short_desc, content_body from articles where publish_date >='" + start_timestamp + "' and publish_date <='" + end_timestamp +"' ");
    res = exec(conn, q.c_str(),  PGRES_TUPLES_OK);

    return res;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  ArticleStoreServiceImpl service;

  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  grpc_compression_options options;
  options.enabled_algorithms_bitset = (1u << GRPC_COMPRESS_ALGORITHMS_COUNT) - 1;
  options.default_compression_algorithm = GRPC_COMPRESS_GZIP;
  std::cout << "OPTIONS:" << options.enabled_algorithms_bitset;
  builder.SetCompressionOptions(options);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  server->Wait();

};

int main(int argc, char* argv[]) {
  openConn("dbname=woi_production", &conn);

  RunServer();
  return 0;
};
