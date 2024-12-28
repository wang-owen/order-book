#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "order.hpp"

#include <deque>
#include <map>
#include <mutex>
#include <unordered_map>

class order_book {
public:
  std::unordered_map<std::string, std::variant<bool, std::string>>
  process_message(const std::string &message, std::string trader_id);
  void add_order(const Order &order);
  void cancel_order(int id);
  void display();

  int num_orders() const;

private:
  std::map<double, std::deque<Order>> bids;
  std::map<double, std::deque<Order>> asks;
  std::mutex mtx;

  void execute_orders();
};

#endif