#include "tcp_connection.hpp"

typedef std::shared_ptr<tcp_connection> pointer;
pointer tcp_connection::create(asio::io_context &io_context, order_book &book) {
  return pointer(new tcp_connection(io_context, book));
}

tcp::socket &tcp_connection::socket() { return socket_; }

void tcp_connection::start() { handle_read(); }

tcp_connection::tcp_connection(asio::io_context &io_context, order_book &book)
    : socket_(io_context), book_(book) {}

void tcp_connection::handle_read() {
  auto self(shared_from_this());

  socket_.async_read_some(
      asio::buffer(buffer_),
      [this, self](std::error_code error, std::size_t length) {
        if (!error) {
          std::cout << "Received: " << std::string(buffer_.data(), length)
                    << std::endl;
          std::unordered_map<std::string, std::variant<bool, std::string>>
              response =
                  book_.process_order(std::string(buffer_.data(), length));

          std::string message;
          if (std::get<bool>(response["success"])) {
            message = "Order processed successfully\n";
          } else {
            message = std::get<std::string>(response["error"]) + "\n";
          }

          asio::async_write(
              socket_, asio::buffer(message),
              [this, self](std::error_code error, std::size_t length) {
                if (error) {
                  std::cerr << "Error: " << error.message() << std::endl;
                }
              });

          handle_read(); // Continue reading for the next message
        } else {
          if (error == asio::error::eof) {
            std::cout << "Connection closed by peer." << std::endl;
          } else {
            std::cerr << "Error: " << error.message() << std::endl;
          }
        }
      });
}