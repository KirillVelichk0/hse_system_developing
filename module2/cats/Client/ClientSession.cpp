#include <Client/ClientSession.hpp>

ClientSessionInternal::ClientSessionInternal(
    boost::asio::any_io_executor &executor)
    : stream_(executor) {}

void ClientSessionInternal::Prepare(SessionParams &&params) {
  if (params.callback == nullptr) {
    throw std::invalid_argument("Client session callback cant be nullptr");
  }
  // Set up an HTTP GET request message
  req_.version(params.version);
  req_.method(std::move(params.reqType));
  req_.target(std::move(params.target));
  req_.body() = std::move(params.body);
  req_.set(http::field::host, std::move(params.host));
  req_.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
  req_.set(http::field::content_type, std::move(params.contentType));
  req_.prepare_payload();
  m_callback = std::move(params.callback);
}

void ClientSessionInternal::on_resolve(beast::error_code ec,
                                       tcp::resolver::results_type results) {

  if (ec) {
    m_callback({}, ec);
    return;
  }

  // Set a timeout on the operation
  stream_.expires_after(std::chrono::seconds(30));

  // Make the connection on the IP address we get from a lookup
  stream_.async_connect(
      results, beast::bind_front_handler(&ClientSessionInternal::on_connect,
                                         shared_from_this()));
}

void ClientSessionInternal::on_connect(
    beast::error_code ec, tcp::resolver::results_type::endpoint_type) {

  if (ec) {
    m_callback({}, ec);
    return;
  }
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

  if (ec) {
    m_callback({}, ec);
    return;
  }

  // Receive the HTTP response
  http::async_read(stream_, buffer_, res_,
                   beast::bind_front_handler(&ClientSessionInternal::on_read,
                                             shared_from_this()));
}

void ClientSessionInternal::on_read(beast::error_code ec,
                                    std::size_t bytes_transferred) {
  boost::ignore_unused(bytes_transferred);

  m_callback(std::move(res_), ec);
}

ClientSessionInternal::~ClientSessionInternal() {
  beast::error_code ec;
  stream_.socket().shutdown(tcp::socket::shutdown_both, ec);
}
