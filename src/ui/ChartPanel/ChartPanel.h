#ifndef CHART_PANEL_H
#define CHART_PANEL_H


#include "../UIState.h"
#include "../../engine/PluginLoader.h"
#include "../../engine/BacktestEngine.h"
#include <vector>


struct StrategyResult {
    std::string name;
    PluginHandle plugin;
    std::vector<float> equity_curve;
};

class ChartPanel {
private:
    std::vector<StrategyResult> loaded_strategies;
    std::vector<Bar>* bars_ptr;  // Pointer to data from main
    
public:
    ChartPanel() : bars_ptr(nullptr) {} 
    void SetBars(std::vector<Bar>* bars);
    void Render(float left_panel_width, float bottom_height, DragState& g_drag_state);
    void RenderCandlesticks(float left_panel_width, float window_height);
};

#endif