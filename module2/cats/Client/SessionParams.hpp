#pragma once
#include <boost/beast/http.hpp>
#include <string>

template <class CallbackType> struct TSessionParams {
  std::string host;
  std::string port;
  std::string target;

  boost::beast::http::verb reqType;
  std::string body;
  std::string contentType;

  std::int32_t version;

  CallbackType callback;
};
