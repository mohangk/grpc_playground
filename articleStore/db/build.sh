c++ -std=c++11 -Wall -g -O0 -Wextra  -DNDEBUG  -fPIC  -c -o articles_query.o articles_query.cc
c++ -std=c++11 -Wall -g -O0 -Wextra  -DNDEBUG  -fPIC articles.cc -lsqlpp-postgresql articles_query.o
