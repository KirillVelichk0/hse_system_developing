#pragma once
#define BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT 1
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/executor.hpp>
#include <functional>

namespace asio = boost::asio;

class IExecutor {
public:
  virtual ~IExecutor();
  virtual void AddTask(std::function<void(asio::any_io_executor)> task) = 0;

  virtual void Stop() = 0;
};
