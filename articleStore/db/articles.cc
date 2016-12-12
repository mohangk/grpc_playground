#include <iostream>
#include "articles.h"
#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>

namespace postgresql = sqlpp::postgresql;

int main(int argc, char* argv[])
{
  if(argc != 4) {
    std::cerr << "We need 3 params: ./a.out <dbname> <host> <user>";
    exit(1);
  }

  auto config = std::make_shared<postgresql::connection_config>();
  config->dbname = argv[1];
  config->host   = argv[2];
  config->user   = argv[3];
  //config->debug = true;

  postgresql::connection db(config);
  const auto t = DB::Articles{};

  for (const auto& row : db(select(t.id, t.title).from(t).unconditionally().limit(10u)))
  {
    int64_t a = row.id;
    std::string b;
    if(!row.title.is_null()) {
       b = row.title;
    } else {
      b = "";
    }
    std::cout << a << ", " << b << std::endl;
  }

  std::cout << "Test";
}
