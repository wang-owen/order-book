#ifndef ORDER_H
#define ORDER_H

#include <ctime>

struct Order {
  int id;
  enum class Type { BUY, SELL } type;
  double price;
  int quantity;
  time_t timestamp;
};

#endif