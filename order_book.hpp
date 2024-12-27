#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "order.hpp"

#include <iostream>
#include <map>
#include <mutex>
#include <unordered_map>
#include <vector>

class order_book {
public:
  std::unordered_map<std::string, std::variant<bool, std::string>>
  process_order(const std::string &message);
  void add_order(const Order &order);
  void cancel_order(int id);
  void display();

  int num_orders() const;

private:
  std::map<double, std::vector<Order>> bids;
  std::map<double, std::vector<Order>> asks;
  std::mutex mtx;

  void execute_orders();
};

#endif