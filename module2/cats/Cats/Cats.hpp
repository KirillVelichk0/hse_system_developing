#pragma once
#include <Cats/IRegistry.hpp>
#include <Client/Client.hpp>
#include <Executor/StrandExecutor.hpp>

#include <memory>
#include <optional>

class CatsProcessor final : std::enable_shared_from_this<CatsProcessor> {
public:
  struct ProcessingStatus {
    std::uint32_t processedCount;
    bool imageAdd;
    std::optional<std::string> zipBody;
  };

public:
  static std::shared_ptr<CatsProcessor>
  Create(std::shared_ptr<StrandExecutor> executor,
         std::unique_ptr<IRegistry> registry);
  ~CatsProcessor();

  inline void SetObserver(std::function<void(ProcessingStatus)> observer) {
    m_statusObserver = std::move(observer);
  }

  CallbackType CreateNewImageProcessor();

public:
  CatsProcessor(const CatsProcessor &) = delete;
  CatsProcessor(CatsProcessor &&) = delete;
  CatsProcessor &operator=(const CatsProcessor &) = delete;
  CatsProcessor &operator=(CatsProcessor &&) = delete;

private:
  CatsProcessor(std::shared_ptr<StrandExecutor> executor,
                std::unique_ptr<IRegistry> registry);

  ProcessingStatus ProcessNewImage(std::string &&data);

private:
  std::shared_ptr<StrandExecutor> m_exec;
  std::unique_ptr<IRegistry> m_registry;
  std::function<void(ProcessingStatus)> m_statusObserver;
};
