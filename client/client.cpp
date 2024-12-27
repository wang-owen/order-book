#include <asio.hpp>
#include <iostream>

int main(int argc, char *argv[]) {
  try {
    if (argc != 3) {
      std::cerr << "Usage: client <host> <port>" << std::endl;
      return 1;
    }

    const std::string host = argv[1];
    const std::string port = argv[2];

    asio::io_context io_context;

    asio::ip::tcp::resolver resolver(io_context);
    asio::ip::tcp::resolver::results_type endpoints =
        resolver.resolve(host, port);

    asio::ip::tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    while (true) {
      std::string input;
      std::cout << "Enter message: ";
      std::getline(std::cin, input);
      asio::write(socket, asio::buffer(input));

      std::array<char, 128> response;
      std::size_t length = socket.read_some(asio::buffer(response));
      std::cout << "Response: " << std::string(response.data(), length)
                << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}