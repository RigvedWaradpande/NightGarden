#include "BacktestEngine.h"

BacktestEngine::BacktestEngine(const std::vector<Bar>& bars, double initial_cash) : bars(bars), portfolio(initial_cash){}

void BacktestEngine::LoadStrategy(IStrategy* strat){
    strategy = strat;
}
void BacktestEngine::Run(){
    for(const auto& bar : bars){
        PortfolioState state = portfolio.GetState(bar);
        Signal signal = strategy->OnBar(bar, state);
        portfolio.ExecuteSignal(signal, bar);
        equity_curve.push_back(portfolio.GetEquity(bar));
    }
}
std::vector<float> BacktestEngine::GetEquityCurve(){
    return equity_curve;
}