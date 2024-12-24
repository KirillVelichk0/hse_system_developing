#include <Cats/Cats.hpp>
#include <Cats/UniqueRegistry.hpp>
#include <Client/Client.hpp>
#include <Executor/BaseExecutor.hpp>
#include <Executor/StrandExecutor.hpp>

#include <atomic>
#include <iostream>
#include <thread>

int main(int argc, char *argv[]) {
  auto base = BaseExecutor::Create(1);
  auto baseAlt = BaseExecutor::Create(1);
  auto strandExec = StrandExecutor::Create(baseAlt);
  auto client = Client::Create(base);
  std::unique_ptr<IRegistry> registry = std::make_unique<UniqueRegistry>();
  auto catsProcessor = CatsProcessor::Create(strandExec, std::move(registry));
  auto getCallback = catsProcessor->CreateNewImageProcessor();

  std::atomic_bool isDone = false;

  CallbackType finalCallback = [base, baseAlt,
                                &isDone](http::response<http::string_body> &&_,
                                         beast::error_code ec) {
    std::cout << "Final" << std::endl;
    isDone.store(true, std::memory_order_release);

    if (ec) {
      std::cout << "Bad final: " << ec.message() << std::endl;
    } else {
      std::cout << "Successfully sended" << std::endl;
    }
    base->Stop();
    baseAlt->Stop();
  };

  std::cout << "Initing params" << std::endl;
  SessionParams params;
  params.host = "algisothal.ru";
  params.port = "8888";
  params.target = "/cat";
  params.version = 11;

  auto statusProcessor =
      [client, processor = catsProcessor, getCallback, finalCallback,
       params](CatsProcessor::ProcessingStatus status) mutable {
        std::cout << "Calling callback" << std::endl;
        if (status.zipBody) {
          std::cout << "Is zip " << status.processedCount << std::endl;
          params.reqType = boost::beast::http::verb::post;
          params.body = std::move(status.zipBody.value());
          params.callback = finalCallback;
          params.contentType = "multipart/form-data";

        } else if (!status.imageAdd) {
          std::cout << "Is not zip and not add " << status.processedCount
                    << std::endl;
          params.reqType = boost::beast::http::verb::get;
          params.callback = getCallback;
        } else {
          std::cout << "Is add " << status.processedCount << std::endl;
          return;
        }
        client->StartSession(params);
      };

  std::cout << "Setting observer" << std::endl;
  catsProcessor->SetObserver(statusProcessor);

  auto startProcess = [params, client, getCallback]() mutable {
    params.reqType = boost::beast::http::verb::get;
    params.callback = getCallback;
    for (int i = 0; i < 12; i++) {
      client->StartSession(params);
    }
  };

  startProcess();
  std::cout << "Started process" << std::endl;

  while (!isDone.load(std::memory_order_acquire)) {
    std::this_thread::yield();
  }
  std::cout << "Done" << std::endl;
  return 0;
}
