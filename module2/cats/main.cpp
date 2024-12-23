#include <Cats/Cats.hpp>
#include <Cats/UniqueRegistry.hpp>
#include <Client/Client.hpp>
#include <Executor/BaseExecutor.hpp>
#include <Executor/StrandExecutor.hpp>

#include <iostream>

int main(int argc, char *argv[]) {
  auto base = BaseExecutor::Create(1);
  auto baseAlt = BaseExecutor::Create(1);
  auto strandExec = StrandExecutor::Create(baseAlt);
  auto client = Client::Create(strandExec);
  std::unique_ptr<IRegistry> registry = std::make_unique<UniqueRegistry>();
  auto catsProcessor = CatsProcessor::Create(strandExec, std::move(registry));
  auto getCallback = catsProcessor->CreateNewImageProcessor();

  CallbackType finalCallback = [base,
                                baseAlt](http::response<http::string_body> &&_,
                                         beast::error_code ec) {
    base->Stop();
    baseAlt->Stop();
    if (ec) {
      throw std::runtime_error("Bad ec");
    }
    std::cout << "Successfully sended" << std::endl;
  };

  SessionParams params;
  params.host = "http://algisothal.ru";
  params.port = "8888";
  params.target = "cat";
  params.version = 11;

  auto statusProcessor =
      [client, processor = catsProcessor, getCallback, finalCallback,
       params](CatsProcessor::ProcessingStatus status) mutable {
        if (status.zipBody) {
          params.reqType = boost::beast::http::verb::post;
          params.body = std::move(status.zipBody.value());
          params.callback = finalCallback;

        } else if (!status.imageAdd) {
          params.reqType = boost::beast::http::verb::get;
          params.callback = getCallback;
        }
      };

  catsProcessor->SetObserver(statusProcessor);

  auto startProcess = [params, client, getCallback]() mutable {
    params.reqType = boost::beast::http::verb::get;
    params.callback = getCallback;
    for (int i = 0; i < 12; i++) {
      client->StartSession(params);
    }
  };

  startProcess();

  return 0;
}
