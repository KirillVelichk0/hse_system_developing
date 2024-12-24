#include <Client/Client.hpp>

std::shared_ptr<Client> Client::Create(std::shared_ptr<IExecutor> executor) {
  return std::shared_ptr<Client>(new Client(std::move(executor)));
}

Client::Client(std::shared_ptr<IExecutor> executor)
    : m_executor(executor), m_resolver(std::make_shared<LazyResolver>()) {}

Client::~Client() = default;

void Client::StartSession(const SessionParams &sessionParams) {
  this->StartSessionImpl(sessionParams);
}

void Client::StartSession(SessionParams &&sessionParams) {
  this->StartSessionImpl(std::move(sessionParams));
}

template <class TSessionParams>
void Client::StartSessionImpl(TSessionParams &&sessionParams) {
  auto task = [params = std::forward<TSessionParams>(sessionParams),
               resolver = m_resolver](asio::any_io_executor executor) mutable {
    std::cout << "Starting task" << std::endl;
    auto clientSession = std::shared_ptr<ClientSessionInternal>(
        new ClientSessionInternal(executor));
    auto host = params.host;
    auto port = params.port;
    clientSession->Prepare(std::move(params));
    resolver->Get(executor)->async_resolve(
        host, port,
        beast::bind_front_handler(&ClientSessionInternal::on_resolve,
                                  clientSession));
  };
  std::cout << "Adding task" << std::endl;
  m_executor->AddTask(std::move(task));
}
