#include "tcp_server.hpp"

tcp_server::tcp_server(unsigned short port, order_book &book)
    : acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)), port_(port),
      book_(book) {
  start_accept();
}

void tcp_server::run() { io_context_.run(); }

void tcp_server::start_accept() {
  tcp_connection::pointer new_connection =
      tcp_connection::create(io_context_, book_);

  acceptor_.async_accept(new_connection->socket(),
                         std::bind(&tcp_server::handle_accept, this,
                                   new_connection, asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection,
                               const std::error_code &error) {
  if (!error) {
    new_connection->start();
  }
  start_accept();
}