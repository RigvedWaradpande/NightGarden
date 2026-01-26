#pragma once
#include <vector>
#include <string>
#include "types.h"
#include "UIState.h"

struct LoadedStrategy {
    std::string name;
    std::vector<float> equity_curve;
    std::vector<Trade> trades;
    PerformanceMetrics metrics;
    bool show_graph;
};

class ChartPanel {
public:
    std::vector<LoadedStrategy> loaded_strategies;
    
    void SetBars(std::vector<Bar>* bars_ptr);
    void RenderMarketData(const std::vector<Bar>& bars);
    void RenderStrategyTab(size_t strategy_index);
    void RemoveStrategy(size_t index);
    void AddStrategy(const std::string& name, 
                     const std::vector<float>& equity,
                     const std::vector<Trade>& trades);
    
private:
    std::vector<Bar>* bars;
    void RenderEquityCurve(const std::vector<float>& equity);
    void RenderMetricsView(const PerformanceMetrics& metrics);
    void RenderCandlesticks(const std::vector<Bar>& bars);
};