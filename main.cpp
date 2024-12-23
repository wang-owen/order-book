#include "common.hpp"

#include "order_book.hpp"

#include <chrono>
#include <unistd.h>

int main() {
  using std::cout;

  OrderBook order_book;

  order_book.AddOrder(
      {10, Order::Type::BUY, 10, 10,
       std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())});
  sleep(1);
  order_book.AddOrder(
      {2, Order::Type::BUY, 100, 10,
       std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())});
  sleep(1);
  order_book.AddOrder(
      {31, Order::Type::BUY, 200, 10,
       std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())});
  sleep(1);

  order_book.AddOrder(
      {19, Order::Type::SELL, 101, 10,
       std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())});
  order_book.AddOrder(
      {19, Order::Type::SELL, 101, 10,
       std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())});

  cout << "\n";
  order_book.Display();
  cout << "\n";

  order_book.AddOrder({5, Order::Type::SELL, 50, 10});

  cout << "\n";
  order_book.Display();
}