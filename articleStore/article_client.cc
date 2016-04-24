#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "article.grpc.pb.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using publishing::Article;
using publishing::ArticleRequest;
using publishing::ArticleStore;

class ArticleClient {
  public:
    ArticleClient(std::shared_ptr<Channel> channel)
      : stub_(ArticleStore::NewStub(channel)) {}

  Article GetArticle(const int articleId) {
    ArticleRequest articleRequest;
    articleRequest.set_id(articleId);

    Article article;

    ClientContext context;

    Status status = stub_->find(&context, articleRequest, &article);

    if(status.ok()) {
      return article;
    } else {
      std::cout << "RPC failed";
      exit(0);
    }
  }

  private:
    std::unique_ptr<ArticleStore::Stub> stub_;
};

int main(int argc, char* argv[]) {

  ArticleClient articleClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
  Article article = articleClient.GetArticle(999);
  std::cout << "title:" << article.title() << std::endl;
  std::cout << "short_desc:" << article.short_desc() << std::endl;
  std::cout << "content_body:" << article.content_body() << std::endl;

  return 0;
}
