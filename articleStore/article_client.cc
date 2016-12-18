#include <memory>
#include <string>
#include <fstream>

#include <grpc++/grpc++.h>

#include "article.grpc.pb.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using grpc::ClientReader;
using publishing::Article;
using publishing::Articles;
using publishing::ArticleStore;

using publishing::ArticleRequest;
using publishing::ArticlesForPeriodRequest;

class ArticleClient {
  public:
    ArticleClient(std::shared_ptr<Channel> channel)
      : stub_(ArticleStore::NewStub(channel)) {}

    void GetArticle(const int articleId) {
      ClientContext context;
      clientContextInit(&context);

      ArticleRequest articleRequest;
      articleRequest.set_id(articleId);

      Article article;

      Status status = stub_->GetArticle(&context, articleRequest, &article);

      if(status.ok()) {
        printArticle(article, 1);
      } else {
        std::cout << "RPC failed";
        exit(0);
      }
    }


    ArticlesForPeriodRequest articlesForPeriodRequestBuilder(std::string start_timestamp, std::string end_timestamp, int category_id)
    {
      ArticlesForPeriodRequest r;

      r.set_start_timestamp(start_timestamp);
      r.set_end_timestamp(end_timestamp);
      r.set_category_id(188);

      return r;
    }


    void ListArticlesBulk(ArticlesForPeriodRequest r) {
      Article a;
      ClientContext context;
      clientContextInit(&context);

      auto benchmark_start = std::chrono::high_resolution_clock::now();

      Articles articles;
      Status status = stub_->ArticlesForPeriodBulk(&context, r, &articles);
      auto benchmark_finish_req = std::chrono::high_resolution_clock::now();

      for(int i = 0; i < articles.article_size(); i++) {
         const Article& a = articles.article(i);
         printArticle(a, i);
      }

      auto benchmark_finish = std::chrono::high_resolution_clock::now();

      if (status.ok()) {
        printTimings(benchmark_start, benchmark_finish_req, benchmark_finish);
      } else {
        std::cout << "RPC failed." << std::endl;
      }
    }

    void ListArticles(ArticlesForPeriodRequest r) {
      Article a;
      ClientContext context;
      clientContextInit(&context);

      auto benchmark_start = std::chrono::high_resolution_clock::now();

      std::unique_ptr<ClientReader<Article>> reader(stub_->ArticlesForPeriod(&context, r));
      auto benchmark_finish_req = std::chrono::high_resolution_clock::now();

      int i = 0;
      while (reader->Read(&a)) {
        i++;
        printArticle(a, i);
      }

      Status status = reader->Finish();

      auto benchmark_finish = std::chrono::high_resolution_clock::now();

      if (status.ok()) {
        printTimings(benchmark_start, benchmark_finish_req, benchmark_finish);
      } else {
        std::cout << "RPC failed." << std::endl;
      }
    }


  private:
    std::unique_ptr<ArticleStore::Stub> stub_;

    void clientContextInit(ClientContext* context) 
    {
        context->set_compression_algorithm(GRPC_COMPRESS_GZIP);
    }

    void printArticle(Article a, int i) 
    {
        std::cout << std::to_string(i) << ":"
                  << a.id() << " : "
                  << a.title() << std::endl;
    }

    void printTimings(std::chrono::high_resolution_clock::time_point start, 
        std::chrono::high_resolution_clock::time_point request_end, 
        std::chrono::high_resolution_clock::time_point complete)
    {
        long long complete_req_time = elapsedTime(start, request_end);
        long long complete_time = elapsedTime(request_end, complete);
        std::cout << "Req time:";
        std::cout << complete_req_time << std::endl;
        std::cout << "Outuput time:";
        std::cout << complete_time << std::endl;
    }

    long long elapsedTime(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end)
    {
      auto elapsed = end - start;
      return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }
};

int main(int argc, char* argv[]) {

  ArticleClient articleClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  // ArticlesForPeriodRequest r;
  //
  // if(argc == 1) {
  //   std::string startTimestamp("2016-01-01 00:00:00");
  //   std::string endTimestamp("2016-01-15 00:00:00");
  //   r = articleClient.articlesForPeriodRequestBuilder(startTimestamp, endTimestamp, 188);
  // } else {
  //   std::string startTimestamp(argv[1]);
  //   std::string endTimestamp(argv[2]);
  //   r = articleClient.articlesForPeriodRequestBuilder(startTimestamp, endTimestamp, 188);
  // }
  //
  //
  // articleClient.ListArticles(r);

  articleClient.GetArticle(999);

  return 0;
}
