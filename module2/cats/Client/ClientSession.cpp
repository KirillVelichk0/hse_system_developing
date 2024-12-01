#include <Client/ClientSession.hpp>

ClientSessionInternal::ClientSessionInternal(asio::executor &executor)
    : stream_(executor) {}

void ClientSessionInternal::Prepare(char const *host, char const *port,
                                    http::verb reqType, char const *target,
                                    int version) {
  // Set up an HTTP GET request message
  req_.version(version);
  req_.method(reqType);
  req_.target(target);
  req_.body();
  req_.set(http::field::host, host);
  req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
}

void ClientSessionInternal::on_resolve(beast::error_code ec,
                                       tcp::resolver::results_type results) {
  if (ec)
    return fail(ec, "resolve");

  // Set a timeout on the operation
  stream_.expires_after(std::chrono::seconds(30));

  // Make the connection on the IP address we get from a lookup
  stream_.async_connect(
      results, beast::bind_front_handler(&ClientSessionInternal::on_connect,
                                         shared_from_this()));
}

void ClientSessionInternal::on_connect(
    beast::error_code ec, tcp::resolver::results_type::endpoint_type) {
  if (ec)
    return fail(ec, "connect");

  // Set a timeout on the operation
  stream_.expires_after(std::chrono::seconds(30));

  // Send the HTTP request to the remote host
  http::async_write(stream_, req_,
                    beast::bind_front_handler(&ClientSessionInternal::on_write,
                                              shared_from_this()));
}

void ClientSessionInternal::on_write(beast::error_code ec,
                                     std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return fail(ec, "write");

  // Receive the HTTP response
  http::async_read(stream_, buffer_, res_,
                   beast::bind_front_handler(&ClientSessionInternal::on_read,
                                             shared_from_this()));
}

void ClientSessionInternal::on_read(beast::error_code ec,
                                    std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  if (ec)
    return fail(ec, "read");

  // Write the message to standard out
  std::cout << res_ << std::endl;

  // Gracefully close the socket
  stream_.socket().shutdown(tcp::socket::shutdown_both, ec);

  // not_connected happens sometimes so don't bother reporting it.
  if (ec && ec != beast::errc::not_connected)
    return fail(ec, "shutdown");

  // If we get here then the connection is closed gracefully
}
