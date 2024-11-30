#pragma once
#include <boost/asio/executor.hpp>

class IResourceProvider {
public:
  virtual ~IResourceProvider();
  virtual boost::asio::executor GetExecutor() = 0;
};
