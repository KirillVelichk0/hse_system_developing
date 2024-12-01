#pragma once
#include <Client/ClientSession.hpp>
#include <Executor/IExecutor.hpp>
class Client {
public:
  void StartSession(char const *host, char const *port, http::verb reqType,
                    char const *target, int version);

private:
  std::shared_ptr<IExecutor> m_executor;
  std::shared_ptr<asio::ip::basic_resolver<asio::ip::tcp, asio::executor>>
      m_resolver;
};
