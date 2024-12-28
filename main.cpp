#include "order.hpp"
#include "order_book.hpp"
#include "tcp_server.hpp"

bool is_valid_port(const std::string &port) {
  return std::all_of(port.begin(), port.end(),
                     [](char c) { return std::isdigit(c); });
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>\n";
    return 1;
  }

  if (!is_valid_port(argv[1])) {
    std::cerr << "Invalid port number\n";
    return 1;
  }

  unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));

  order_book book;

  tcp_server server(port, book);
  std::cout << "Server started on port " << port << "\n";
  server.run();
}