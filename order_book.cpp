#include "order_book.hpp"

#include <iostream>
#include <sstream>

std::unordered_map<std::string, std::variant<bool, std::string>>
order_book::process_message(const std::string &message, std::string trader_id) {
  std::string type = message.substr(0, message.find(' '));

  // Message format: <BUY, SELL> <price> <quantity> or CANCEL <id>
  if (type == "BUY" || type == "SELL") {
    std::istringstream iss(message);
    double price;
    int quantity;

    iss >> type >> price >> quantity;

    if (iss.fail()) {
      return {{"success", false}, {"error", "Invalid message format"}};
    }

    Order::Type order_type;
    if (type == "BUY") {
      order_type = Order::Type::BUY;
    } else {
      order_type = Order::Type::SELL;
    }
    struct Order order = {num_orders() + 1, order_type,    price,
                          quantity,         time(nullptr), trader_id};

    std::lock_guard<std::mutex> lock(mtx);
    add_order(order);
  } else if (type == "CANCEL") {
    int id = std::stoi(message.substr(message.find(' ') + 1));
    std::lock_guard<std::mutex> lock(mtx);
    cancel_order(id);
  } else {
    return {{"success", false}, {"error", "Invalid message type"}};
  }

  display();

  return {{"success", true}, {"message", "Order processed"}};
}

void order_book::add_order(const Order &order) {
  if (order.type == Order::Type::BUY) {
    bids[order.price].push_back(order);
    std::cout << "Buy Order: Price " << order.price << " Quantity "
              << order.quantity << " Time " << order.timestamp << "\n";
  } else {
    asks[order.price].push_back(order);
    std::cout << "Sell Order: Price " << order.price << " Quantity "
              << order.quantity << " Time " << order.timestamp << "\n";
  }
  execute_orders();
}

void order_book::cancel_order(int id) {
  for (auto &bid : bids) {
    for (auto i = bid.second.begin(); i != bid.second.end(); ++i) {
      if (i->id == id) {
        bid.second.erase(i);
        std::cout << "Order " << id << " cancelled\n";
        if (bid.second.empty()) {
          bids.erase(bid.first);
        }
        return;
      }
    }
  }

  for (auto &ask : asks) {
    for (auto i = ask.second.begin(); i != ask.second.end(); ++i) {
      if (i->id == id) {
        ask.second.erase(i);
        if (ask.second.empty()) {
          asks.erase(ask.first);
        }
        return;
      }
    }
  }
}

void order_book::display() {
  std::cout << "Bids:\n";
  for (const auto &bid : bids) {
    for (const auto &order : bid.second) {
      std::cout << "Price " << bid.first << " Quantity " << order.quantity
                << " Time " << order.timestamp << "\n";
    }
  }

  std::cout << "Asks:\n";
  for (const auto &ask : asks) {
    for (const auto &order : ask.second) {
      std::cout << "Price " << ask.first << " Quantity " << order.quantity
                << " Time " << order.timestamp << " Trader " << order.trader_id
                << "\n";
    }
  }
}

int order_book::num_orders() const {
  int num = 0;
  for (const auto &bid : bids) {
    num += bid.second.size();
  }
  for (const auto &ask : asks) {
    num += ask.second.size();
  }
  return num;
}

void order_book::execute_orders() {
  while (!bids.empty() && !asks.empty()) {
    auto best_bid = bids.rbegin(); // Best BUY price
    auto best_ask = asks.begin();  // Best SELL price

    if (best_ask->first > best_bid->first) {
      break;
    }

    auto &buy_queue = best_bid->second;
    auto &sell_queue = best_ask->second;

    Order &buy_order = buy_queue.front();
    Order &sell_order = sell_queue.front();

    int traded_quantity = std::min(buy_order.quantity, sell_order.quantity);

    // Process trade
    buy_order.quantity -= traded_quantity;
    sell_order.quantity -= traded_quantity;

    if (buy_order.quantity == 0)
      buy_queue.pop_front();
    if (sell_order.quantity == 0)
      sell_queue.pop_front();

    // Clean up empty price levels
    if (buy_queue.empty())
      bids.erase(best_bid->first);
    if (sell_queue.empty())
      asks.erase(best_ask->first);

    std::cout << "Trade: Price " << best_ask->first << " Quantity "
              << traded_quantity << "\n";
  }
}