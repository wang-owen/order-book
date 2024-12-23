#include "order_book.hpp"

void OrderBook::AddOrder(const Order &order) {
  if (order.type == Order::Type::BUY) {
    bids[order.price].push_back(order);
    std::cout << "Buy Order: Price " << order.price << " Quantity "
              << order.quantity << " Time " << order.timestamp << "\n";
  } else {
    asks[order.price].push_back(order);
    std::cout << "Sell Order: Price " << order.price << " Quantity "
              << order.quantity << " Time " << order.timestamp << "\n";
  }
  ExecuteOrders();
}

void OrderBook::CancelOrder(int id) {
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

void OrderBook::Display() {
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

void OrderBook::ExecuteOrders() {
  while (!bids.empty() && !asks.empty()) {
    auto best_bid = bids.rbegin();
    auto best_ask = asks.begin();

    if (best_bid->first < best_ask->first) {
      break;
    }

    auto &bid_orders = best_bid->second.front();
    auto &ask_order = best_ask->second.front();

    int traded_quantity = std::min(bid_orders.quantity, ask_order.quantity);
    std::cout << "Trade: Price " << best_ask->first << " Quantity "
              << traded_quantity << "\n";

    bid_orders.quantity -= traded_quantity;
    ask_order.quantity -= traded_quantity;

    if (bid_orders.quantity == 0) {
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