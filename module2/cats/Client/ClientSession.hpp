#pragma once
#include <boost/beast/http/buffer_body.hpp>
#define BOOST_ASIO_USE_TS_EXECUTOR_AS_DEFAULT 1

#include <boost/beast/core.hpp>

#include <boost/asio/any_io_executor.hpp>

#include <Client/SessionParams.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/version.hpp>
#include <cstdlib>
#include <functional>
#include <memory>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

using CallbackType = std::function<void(http::response<http::string_body> &&,
                                        beast::error_code)>;
using SessionParams = TSessionParams<CallbackType>;

class ClientSessionInternal final
    : public std::enable_shared_from_this<ClientSessionInternal> {
public:
  friend class Client;

  void Prepare(SessionParams &&params);

  ~ClientSessionInternal();

private:
  explicit ClientSessionInternal(boost::asio::any_io_executor &executor);

  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

  void on_connect(beast::error_code ec,
                  tcp::resolver::results_type::endpoint_type);

  void on_write(beast::error_code ec, std::size_t bytes_transferred);

  void on_read(beast::error_code ec, std::size_t bytes_transferred);

private:
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_; // (Must persist between reads)
  http::request<http::string_body> req_;
  http::response<http::string_body> res_;
  CallbackType m_callback;
};
