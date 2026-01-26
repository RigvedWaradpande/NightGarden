#pragma once
#include <string>
#include <vector>

struct Bar {
    int timestamp;
    float open;
    float high;
    float low;
    float close;
    float volume;
    std::string date;
};

struct Trade {
    int timestamp;
    enum Type { BUY, SELL } type;
    float price;
    int quantity;
    float pnl;
};

struct PerformanceMetrics {
    float total_return;
    float sharpe_ratio;
    float max_drawdown;
    float win_rate;
    int num_trades;
};

enum Signal {
    BUY,
    SELL,
    HOLD
};

struct PortfolioState {
    float cash;
    float equity;
    int position;
    float unrealized_pnl;
};