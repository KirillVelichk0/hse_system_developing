#pragma once
#include <Client/ClientSession.hpp>
#include <Executor/IExecutor.hpp>
#include <mutex>

class Client {
public:
  static std::shared_ptr<Client> Create(std::shared_ptr<IExecutor> executor);

  ~Client();

  void StartSession(const SessionParams &sessionParams);
  void StartSession(SessionParams &&sessionParams);

public:
  Client(const Client &) = delete;
  Client(Client &&) = delete;
  Client &operator=(const Client &) = delete;
  Client &operator=(Client &&) = delete;

private:
  Client(std::shared_ptr<IExecutor> executor);

  template <class TSessionParams>
  void StartSessionImpl(TSessionParams &&sessionParams);

  using BasicResolver = asio::ip::basic_resolver<asio::ip::tcp, asio::executor>;

  class LazyResolver {

  public:
    std::shared_ptr<BasicResolver> Get(asio::any_io_executor &executor) {
      std::call_once(m_initFlag,
                     [&executor, this]() { this->InitOnce(executor); });
      return m_resolver;
    }

  private:
    void InitOnce(asio::any_io_executor &executor) {
      m_resolver = std::make_shared<BasicResolver>(executor);
    }

  private:
    std::shared_ptr<BasicResolver> m_resolver;
    std::once_flag m_initFlag;
  };

private:
  std::shared_ptr<IExecutor> m_executor;
  std::shared_ptr<LazyResolver> m_resolver;
};
