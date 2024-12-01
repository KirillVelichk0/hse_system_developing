#include <Client/Client.hpp>

void Client::StartSession(char const *host, char const *port,
                          http::verb reqType, char const *target, int version) {
  auto task = [host, port, reqType, target, version,
               resolver = m_resolver](asio::executor executor) {
    auto clientSession = std::shared_ptr<ClientSessionInternal>(
        new ClientSessionInternal(executor));
    clientSession->Prepare(host, port, reqType, target, version);
    resolver->async_resolve(
        host, port,
        beast::bind_front_handler(&ClientSessionInternal::on_resolve,
                                  clientSession));
  };
  m_executor->AddTask(std::move(task));
}
