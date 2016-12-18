#include <iostream>
#include "db/articles_query.h"

int main(int argc, char* argv[])
{
  if(argc != 4) {
    std::cerr << "We need 3 params: ./a.out <dbname> <host> <user>";
    exit(1);
  }

  auto q = ArticlesQuery();
  auto a = Article();
  q.find(999, &a);
  std::cout << a.title();
  // auto db = q.connect();
  // const auto t = DB::Articles{};
  //
  // for (const auto& row : db(select(t.id, t.title).from(t).unconditionally().limit(10u)))
  // {
  //   int64_t a = row.id;
  //   std::string b;
  //   if(!row.title.is_null()) {
  //      b = row.title;
  //   } else {
  //     b = "";
  //   }
  //   std::cout << a << ", " << b << std::endl;
  // }

  std::cout << "Test";
}
