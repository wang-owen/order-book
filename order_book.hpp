#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "order.hpp"

#include <iostream>
#include <map>
#include <vector>

class OrderBook {
public:
  void AddOrder(const Order &order);
  void CancelOrder(int id);
  void Display();

private:
  std::map<double, std::vector<Order>> bids;
  std::map<double, std::vector<Order>> asks;

  void ExecuteOrders();
};

#endif