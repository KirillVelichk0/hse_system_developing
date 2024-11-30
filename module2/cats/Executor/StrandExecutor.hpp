#pragma once
#include <Executor/BaseExecutor.hpp>
#include <Executor/IExecutor.hpp>
#include <boost/asio/strand.hpp>

class StrandExecutor : public IExecutor {
public:
  // Тут было бы неплохо подружить все с boost::asio::executor, но это хлопотно
  static std::shared_ptr<StrandExecutor>
  Create(std::shared_ptr<BaseExecutor> base);

  ~StrandExecutor() override;
  void AddTask(std::function<void()> task) override;
  void Stop() override;

public:
  StrandExecutor(const StrandExecutor &) = delete;
  StrandExecutor(StrandExecutor &&) = delete;
  StrandExecutor &operator=(const StrandExecutor &) = delete;
  StrandExecutor &operator=(StrandExecutor &&) = delete;

private:
  StrandExecutor(std::shared_ptr<BaseExecutor> base);

private:
  std::shared_ptr<BaseExecutor> m_base;
  boost::asio::strand<boost::asio::io_service::executor_type> m_strand;
};
