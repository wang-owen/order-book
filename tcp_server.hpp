#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "order_book.hpp"
#include "tcp_connection.hpp"

using asio::ip::tcp;

class tcp_server {
public:
  tcp_server(unsigned short port, order_book &book);

  void run();

private:
  void start_accept();

  void handle_accept(tcp_connection::pointer new_connection,
                     const std::error_code &error);

  asio::io_context io_context_;
  tcp::acceptor acceptor_;
  unsigned short port_;
  order_book &book_;
};

#endif