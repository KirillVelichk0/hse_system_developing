#pragma once
#include <boost/asio/io_service.hpp>

class IResourceProvider {
public:
  virtual ~IResourceProvider();
  virtual boost::asio::io_service &GetService();
};
