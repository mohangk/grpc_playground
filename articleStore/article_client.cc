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

    Article GetArticle(const int articleId) {
      ArticleRequest articleRequest;
      articleRequest.set_id(articleId);

      Article article;

      ClientContext context;

      Status status = stub_->GetArticle(&context, articleRequest, &article);

      if(status.ok()) {
        return article;
      } else {
        std::cout << "RPC failed";
        exit(0);
      }
    }


    void ListArticlesBulk(std::string start_timestamp, std::string end_timestamp, int category_id) {
      Article a;
      ClientContext context;
      ArticlesForPeriodRequest r;

      r.set_start_timestamp(start_timestamp);
      r.set_end_timestamp(end_timestamp);
      r.set_category_id(188);

      auto benchmark_start = std::chrono::high_resolution_clock::now();
      context.set_compression_algorithm(GRPC_COMPRESS_GZIP);

      Articles articles;

      Status status = stub_->ArticlesForPeriodBulk(&context, r, &articles);

      auto benchmark_finish_req = std::chrono::high_resolution_clock::now();

      for(int i = 0; i < articles.article_size(); i++) {
         const Article& a = articles.article(i);
         std::cout << std::to_string(i) << ":"
                   << a.id() << " : "
                   << a.title() << std::endl;
      }

      auto benchmark_finish = std::chrono::high_resolution_clock::now();


      if (status.ok()) {
        long long complete_req_time = elapsedTime(benchmark_start, benchmark_finish_req);
        long long complete_time = elapsedTime(benchmark_finish_req, benchmark_finish);
        std::cout << "Req time:";
        std::cout << complete_req_time << std::endl;
        std::cout << "Outuput time:";
        std::cout << complete_time << std::endl;
      } else {
        std::cout << "RPC failed." << std::endl;
      }
    }

    void ListArticles(std::string start_timestamp, std::string end_timestamp, int category_id) {
      Article a;
      ClientContext context;
      ArticlesForPeriodRequest r;

      r.set_start_timestamp(start_timestamp);
      r.set_end_timestamp(end_timestamp);
      r.set_category_id(188);

      auto benchmark_start = std::chrono::high_resolution_clock::now();

      context.set_compression_algorithm(GRPC_COMPRESS_GZIP);

      std::unique_ptr<ClientReader<Article>> reader(stub_->ArticlesForPeriod(&context, r));

      auto benchmark_finish_req = std::chrono::high_resolution_clock::now();

      int i = 0;

      while (reader->Read(&a)) {
        i++;
        std::cout << std::to_string(i) << ":"
                  << a.id() << " : "
                  << a.title() << std::endl;
      }

      auto benchmark_finish = std::chrono::high_resolution_clock::now();

      Status status = reader->Finish();

      if (status.ok()) {
        long long complete_req_time = elapsedTime(benchmark_start, benchmark_finish_req);
        long long complete_time = elapsedTime(benchmark_finish_req, benchmark_finish);
        std::cout << "Req time:";
        std::cout << complete_req_time << std::endl;
        std::cout << "Outuput time:";
        std::cout << complete_time << std::endl;
      } else {
        std::cout << "RPC failed." << std::endl;
      }
    }

    void PrintArticle(int article_id)
    {
      Article article = GetArticle(999);
      std::cout << "title:" << article.title() << std::endl;
      std::cout << "short_desc:" << article.short_desc() << std::endl;
      std::cout << "content_body:" << article.content_body() << std::endl;
    }


  private:

    std::unique_ptr<ArticleStore::Stub> stub_;

    long long elapsedTime(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end)
    {
      auto elapsed = end - start;
      return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }
};

int main(int argc, char* argv[]) {

  ArticleClient articleClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  articleClient.ListArticlesBulk("2016-01-15 00:00:00", "2016-01-31 23:59:59", 188);
  //articleClient.PrintArticle(999);

  return 0;
}
