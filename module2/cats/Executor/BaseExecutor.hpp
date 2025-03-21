#pragma once
#include <Executor/IExecutor.hpp>
#include <Executor/IResourceProvider.hpp>
#include <boost/asio/executor.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>
#include <memory>

class BaseExecutor final : public IExecutor, public IResourceProvider {
public:
  static std::shared_ptr<BaseExecutor> Create(std::size_t poolSize);

  ~BaseExecutor() override;
  void AddTask(std::function<void(asio::any_io_executor)> task) override;
  void Stop() override;

  boost::asio::any_io_executor GetExecutor() override;

public:
  BaseExecutor(const BaseExecutor &) = delete;
  BaseExecutor &operator=(const BaseExecutor &) = delete;
  BaseExecutor(BaseExecutor &&) = delete;
  BaseExecutor &operator=(BaseExecutor &&) = delete;

private:
  BaseExecutor(std::size_t pool_size);

private:
  boost::thread_group m_group;
  boost::asio::io_service m_service;
  asio::executor_work_guard<asio::io_context::executor_type> m_guard;
};
