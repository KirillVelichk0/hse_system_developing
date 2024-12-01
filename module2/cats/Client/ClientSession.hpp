#include <Executor/StrandExecutor.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/version.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

// Report a failure
void fail(beast::error_code ec, char const *what) {
  std::cerr << what << ": " << ec.message() << "\n";
}

class ClientSessionInternal
    : public std::enable_shared_from_this<ClientSessionInternal> {
  beast::tcp_stream stream_;
  beast::flat_buffer buffer_; // (Must persist between reads)
  http::request<http::string_body> req_;
  http::response<http::string_body> res_;

public:
  friend class Client;

  void Prepare(char const *host, char const *port, http::verb reqType,
               char const *target, int version);

private:
  explicit ClientSessionInternal(asio::executor &executor);

  void on_resolve(beast::error_code ec, tcp::resolver::results_type results);

  void on_connect(beast::error_code ec,
                  tcp::resolver::results_type::endpoint_type);

  void on_write(beast::error_code ec, std::size_t bytes_transferred);

  void on_read(beast::error_code ec, std::size_t bytes_transferred);
};
