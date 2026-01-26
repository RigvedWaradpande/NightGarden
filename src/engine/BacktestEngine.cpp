#include "BacktestEngine.h"
#include <iostream>

BacktestEngine::BacktestEngine(const std::vector<Bar>& bars, double initial_cash) : bars(bars), portfolio(initial_cash){}

void BacktestEngine::LoadStrategy(IStrategy* strat){
    strategy = strat;
}
void BacktestEngine::Run() {
    std::cout << "=== RUN START ===" << std::endl;
    std::cout << "bars.size() = " << bars.size() << std::endl;
    std::cout << "strategy pointer = " << strategy << std::endl;
    
    for(size_t i = 0; i < bars.size(); i++) {
        PortfolioState state = portfolio.GetState(bars[i]);
        
        Signal signal = strategy->OnBar(bars[i], state);
        
        portfolio.ExecuteSignal(signal, bars[i]);
        
        equity_curve.push_back(portfolio.GetEquity(bars[i]));
    }
    std::cout << "=== RUN COMPLETE ===" << std::endl;
}
std::vector<float> BacktestEngine::GetEquityCurve(){
    return equity_curve;
}