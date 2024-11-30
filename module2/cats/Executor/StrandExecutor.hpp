#pragma once
#include <Executor/IExecutor.hpp>
#include <Executor/IResourceProvider.hpp>
#include <atomic>
#include <boost/asio/strand.hpp>

class StrandExecutor : public IExecutor {
public:
  static std::shared_ptr<StrandExecutor>
  Create(std::shared_ptr<IResourceProvider> base);

  ~StrandExecutor() override;
  void AddTask(std::function<void()> task) override;
  void Stop() override;

public:
  StrandExecutor(const StrandExecutor &) = delete;
  StrandExecutor(StrandExecutor &&) = delete;
  StrandExecutor &operator=(const StrandExecutor &) = delete;
  StrandExecutor &operator=(StrandExecutor &&) = delete;

private:
  StrandExecutor(std::shared_ptr<IResourceProvider> base);

private:
  std::shared_ptr<IResourceProvider> m_provider;
  boost::asio::strand<boost::asio::io_service::executor_type> m_strand;
  std::atomic<bool> m_isRun{true};
};
