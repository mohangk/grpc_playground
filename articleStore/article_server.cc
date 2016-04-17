#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "article.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using publishing::Article;
using publishing::ArticleRequest;
using publishing::ArticleStore;

class ArticleStoreServiceImpl final : public ArticleStore::Service {
  Status find(ServerContext* context, const ArticleRequest* articleRequest, Article* article) override {
    std::string fakeTitle("Fake title");
    std::string fakeShortDesc("Fake short desc");
    std::string fakeContentBody("Fake content body");

    article->set_id(articleRequest->id());
    article->set_title(fakeTitle);
    article->set_short_desc(fakeShortDesc);
    article->set_content_body(fakeContentBody);

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
