#pragma once
#include <boost/asio/executor.hpp>
#include <functional>

namespace asio = boost::asio;

class IExecutor {
public:
  virtual ~IExecutor();
  virtual void AddTask(std::function<void(asio::executor)> task) = 0;

  virtual void Stop() = 0;
};
