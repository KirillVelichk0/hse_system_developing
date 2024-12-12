#pragma once
#include <boost/asio/any_io_executor.hpp>

class IResourceProvider {
public:
  virtual ~IResourceProvider();
  virtual boost::asio::any_io_executor GetExecutor() = 0;
};
