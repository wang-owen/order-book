#include <asio.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    if (argc != 4) {
      std::cerr << "Usage: client <host> <port> <trader_id>\n";
      return 1;
    }

    const std::string host = argv[1];
    const std::string port = argv[2];
    const std::string trader_id = argv[3];

    // Connect to server
    asio::io_context io_context;

    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoints =
        resolver.resolve(host, port);

    asio::ip::tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    // Send trader ID to server
    asio::write(socket, asio::buffer(trader_id));
    std::array<char, 128> response;
    std::size_t length = socket.read_some(asio::buffer(response));
    if (response.data()[0] == '0') {
      std::cerr << "Invalid trader ID\n";
      return 1;
    }
    std::cout << std::string(response.data(), length) << "\n";

    // Send messages to server
    while (true) {
      std::string input;
      std::cout << "Enter message: ";
      std::getline(std::cin, input);
      asio::write(socket, asio::buffer(input));

      length = socket.read_some(asio::buffer(response));
      std::cout << "Response: " << std::string(response.data(), length) << "\n";
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << "\n";
  }
}