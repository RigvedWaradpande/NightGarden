#include "Portfolio.h"

Portfolio::Portfolio(double initial_cash) : cash(initial_cash), position(0){}

void Portfolio::ExecuteSignal(Signal signal, const Bar& bar){
    if(signal == Signal::BUY && position == 0){
        //buy as much as possible
        avg_entry_price = bar.close;
        position = cash/avg_entry_price;
        cash = cash - (cash/avg_entry_price)*avg_entry_price;
    }
    else if(signal == Signal::SELL && position > 0){
        //sell all
        cash = cash + bar.close * position;
        position = 0;
        avg_entry_price = 0;
    }
    else{
        //do nothing
    }
}

PortfolioState Portfolio::GetState(const Bar& current_bar) {
    PortfolioState state;
    state.cash = cash;
    state.position = position;
    state.equity = GetEquity(current_bar);
    state.unrealized_pnl = (position > 0) ? position * (current_bar.close - avg_entry_price) : 0.0;
    return state;
}

double Portfolio::GetEquity(const Bar& current_bar){
    double Equity = cash + (position * current_bar.close);
    return Equity;
}