#ifndef STRATEGY_PANEL_H
#define STRATEGY_PANEL_H

#include <string>
#include <vector>
#include "../UIState.h"

class StrategyPanel{
    public:
        void Render(DragState& g_drag_state);
        std::vector<std::string> DiscoverStrategies(const std::string& plugin_dir);
};

#endif