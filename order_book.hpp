#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "order.hpp"

#include <deque>
#include <map>
#include <mutex>

class order_book {
public:
  void process_message(const std::string &message, std::string &trader_id,
                       std::string &response);
  void add_order(const Order &order);
  bool cancel_order(int id, std::string &trader_id);
  void display();
  void display(std::string &trader_id, std::string &out);

  int num_orders() const;

private:
  std::map<double, std::deque<Order>> bids;
  std::map<double, std::deque<Order>> asks;
  std::mutex mtx;

  void execute_orders();
};

#endif