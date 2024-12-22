#pragma once
#include <Cats/IRegistry.hpp>
#include <Client/Client.hpp>
#include <Executor/StrandExecutor.hpp>

class CatsProcessor {
public:
  void ProcessNewImage(std::string &&data);

private:
  StrandExecutor m_exec;
  std::unique_ptr<IRegistry> m_registry;
  std::shared_ptr<Client> m_client;
};
