#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "../include/types.h"

class Portfolio {
private:
    double cash;
    int position;           // Number of shares held
    double avg_entry_price; // Average price we bought at
    
public:
    Portfolio(double initial_cash);
    void ExecuteSignal(Signal signal, const Bar& bar);
    PortfolioState GetState(const Bar& current_bar);
    double GetEquity(const Bar& current_bar);
};

#endif