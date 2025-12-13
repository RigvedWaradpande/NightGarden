#ifndef BACKTESTENGINE_H
#define BACKTESTENGINE_H

#include <vector>
#include "../include/types.h"
#include "Portfolio.h"
#include "../include/IStrategy.h"

class BacktestEngine {
private:
    std::vector<Bar> bars;
    Portfolio portfolio;
    IStrategy* strategy;
    std::vector<float> equity_curve;  // For plotting
    
public:
    BacktestEngine(const std::vector<Bar>& bars, double initial_cash);
    void LoadStrategy(IStrategy* strat);
    void Run();
    std::vector<float> GetEquityCurve();
};

#endif