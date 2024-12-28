#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <array>
#include <asio.hpp>
#include <iostream>
#include <memory>

#include "order_book.hpp"

using asio::ip::tcp;

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
  typedef std::shared_ptr<tcp_connection> pointer;
  static pointer create(asio::io_context &io_context, order_book &book);

  tcp::socket &socket();

  void start();

private:
  tcp_connection(asio::io_context &io_context, order_book &book);

  void handle_read();

  tcp::socket socket_;
  std::array<char, 128> buffer_;
  order_book &book_;
  std::string trader_id_;
};

#endif