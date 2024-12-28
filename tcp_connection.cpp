#include "tcp_connection.hpp"

typedef std::shared_ptr<tcp_connection> pointer;
pointer tcp_connection::create(asio::io_context &io_context, order_book &book) {
  return pointer(new tcp_connection(io_context, book));
}

tcp::socket &tcp_connection::socket() { return socket_; }

void tcp_connection::start() {
  // Validate trader ID
  bool valid_id = true;
  socket_.async_read_some(
      asio::buffer(buffer_),
      [this, &valid_id](std::error_code error, std::size_t length) {
        if (!error) {
          const std::string trader_id = std::string(buffer_.data(), length);

          valid_id = std::all_of(trader_id.begin(), trader_id.end(),
                                 [](char c) { return std::isalnum(c); });

          std::string message;
          if (valid_id) {
            std::cout << trader_id << " has connected" << "\n";
            message = std::format("Hello, {}!\n", trader_id);
            trader_id_ = trader_id;
          } else {
            message = "0";
          }

          asio::async_write(socket_, asio::buffer(message),
                            [this](std::error_code error, std::size_t length) {
                              if (error) {
                                std::cerr << "Error: " << error.message()
                                          << "\n";
                              }
                            });
        } else {
          std::cerr << "Error: " << error.message() << "\n";
        }
      });

  if (valid_id) {
    handle_read();
  }
}

tcp_connection::tcp_connection(asio::io_context &io_context, order_book &book)
    : socket_(io_context), book_(book) {}

void tcp_connection::handle_read() {
  // Capture this pointer to keep it alive
  auto self(shared_from_this());

  // Read message from client
  socket_.async_read_some(asio::buffer(buffer_), [this,
                                                  self](std::error_code error,
                                                        std::size_t length) {
    if (!error) {
      std::cout << "Received: " << std::string(buffer_.data(), length) << "\n";

      // Process message and get response
      std::unordered_map<std::string, std::variant<bool, std::string>>
          response = book_.process_message(std::string(buffer_.data(), length),
                                           trader_id_);

      std::string message;
      if (std::get<bool>(response["success"])) {
        message = std::get<std::string>(response["message"]) + "\n";
      } else {
        message = std::get<std::string>(response["error"]) + "\n";
      }

      asio::async_write(
          socket_, asio::buffer(message),
          [this, self](std::error_code error, std::size_t length) {
            if (error) {
              std::cerr << "Error: " << error.message() << "\n";
            }
          });

      handle_read(); // Continue reading for the next message
    } else {
      if (error == asio::error::eof) {
        std::cout << trader_id_ << " disconnected\n";
      } else {
        std::cerr << "Error: " << error.message() << "\n";
      }
    }
  });
}