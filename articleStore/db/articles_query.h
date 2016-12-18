#include <sqlpp11/sqlpp11.h>
#include <sqlpp11/postgresql/postgresql.h>
#include "../article.grpc.pb.h"

using publishing::Article;
namespace postgresql = sqlpp::postgresql;

class ArticlesQuery {

  public:
    ArticlesQuery(void);
    void find(const int id, Article* article);
    postgresql::connection connect(void);
};
