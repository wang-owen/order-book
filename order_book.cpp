#include "order_book.hpp"

#include <sstream>

std::unordered_map<std::string, std::variant<bool, std::string>>
order_book::process_order(const std::string &message) {
  // Message format: <type> <price> <quantity>
  std::istringstream iss(message);
  std::string type;
  double price;
  int quantity;
  std::string timestamp;

  iss >> type >> price >> quantity;

  if (iss.fail()) {
    return {{"success", false}, {"error", "Invalid message format"}};
  }

  Order::Type order_type;
  if (type == "BUY") {
    order_type = Order::Type::BUY;
  } else if (type == "SELL") {
    order_type = Order::Type::SELL;
  } else {
    return {{"success", false}, {"error", "Invalid order type"}};
  }

  struct Order order = {num_orders() + 1, order_type, price, quantity,
                        time(nullptr)};

  std::lock_guard<std::mutex> lock(mtx);
  add_order(order);

  std::cout << "Order: Type " << type << " Price " << price << " Quantity "
            << quantity << "\n";

  display();

  return {{"success", true}, {"error", ""}};
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
                << " Time " << order.timestamp << "\n";
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
    auto best_bid = bids.rbegin(); // Highest bid
    auto best_ask = asks.begin();  // Lowest ask

    // Lowest ask is greater than highest bid
    if (best_ask->first > best_bid->first) {
      break;
    }

    auto &bid_order = best_bid->second.front();
    auto &ask_order = best_ask->second.front();

    int traded_quantity = std::min(bid_order.quantity, ask_order.quantity);
    std::cout << "Trade: Price " << best_ask->first << " Quantity "
              << traded_quantity << "\n";

    bid_order.quantity -= traded_quantity;
    ask_order.quantity -= traded_quantity;

    if (bid_order.quantity == 0) {
      best_bid->second.erase(best_bid->second.begin());
      if (best_bid->second.empty()) {
        bids.erase(best_bid->first);
      }
    }

    if (ask_order.quantity == 0) {
      best_ask->second.erase(best_ask->second.begin());
      if (best_ask->second.empty()) {
        asks.erase(best_ask->first);
      }
    }
  }
}