#include <iostream>
#include <sqlpp11/result.h>
#include "articles.h"
#include "articles_query.h"
#include "../article.grpc.pb.h"

using publishing::Article;

ArticlesQuery::ArticlesQuery()  {

  //config->debug =// true;

}


postgresql::connection ArticlesQuery::connect() {
  auto config = std::make_shared<postgresql::connection_config>();
  config->dbname = "woi_production"; //argv[1];
  config->host   = "localhost";//argv[2];
  config->user   = "mohangk";//argv[3];
  
  try  {
    postgresql::connection _db(config);
    return _db;
  } catch(sqlpp::exception err) {
    std::cout << err.what();
  }
}

// void populateArticleFromRS(Article* a, PGresult *res, int i)
// {
//     int id = std::stoi(PQgetvalue(res, i, 0));
//     std::string title(PQgetvalue(res, i, 1));
//     std::string short_desc(PQgetvalue(res, i, 2));
//     std::string content_body(PQgetvalue(res, i, 3));
//
//     a->set_id(id);
//     a->set_title(title);
//     a->set_short_desc(short_desc);
//     a->set_content_body(content_body);
// }

void ArticlesQuery::find(int articleId, Article* a) {
  const auto t = DB::Articles{};
  auto db = connect();

  auto row = db(select(t.id, t.title, t.shortDesc, t.contentBody).from(t).where(t.id == articleId ));
  
  a->set_id(row.begin()->id);
  a->set_title(row.begin()->title);
  a->set_short_desc(row.begin()->shortDesc);
  a->set_content_body(row.begin()->contentBody);

}


