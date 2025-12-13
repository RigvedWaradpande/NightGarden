#ifndef TYPES_H
#define TYPES_H

#include <string>
#include "imgui.h"

struct Bar {
    long timestamp;
    std::string date;
    double open;
    double high;
    double low;
    double close;
    long volume;
};

struct PortfolioState {
    double cash;
    double equity;
    int position;
    double unrealized_pnl;
};

enum class Signal {
    BUY,
    SELL,
    HOLD
};

#endif