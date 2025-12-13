#include "ChartPanel.h"
#include <iostream>
#include "imgui.h"

void ChartPanel::Render(float left_panel_width, float bottom_height, DragState& g_drag_state) {
    ImGuiIO& io = ImGui::GetIO();
    
    float candlestick_height = 300.0f;  // Fixed height for now
    RenderCandlesticks(left_panel_width, candlestick_height);
    
    // Adjust remaining space for strategy tiles
    float total_height = io.DisplaySize.y - 20 - bottom_height - candlestick_height;
    int num_strategies = loaded_strategies.size();

    // Always render at least one window for dropping
    if (num_strategies == 0) {
        ImGui::SetNextWindowPos(ImVec2(left_panel_width, 20 + candlestick_height)); // OFFSET HERE
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - left_panel_width, total_height));
        ImGui::Begin("Drop Zone", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        if (g_drag_state.is_dragging) {
            ImGui::Text("Dragging: %s", g_drag_state.strategy_name.c_str());
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddCircleFilled(g_drag_state.drag_pos, 10.0f, IM_COL32(255, 0, 0, 255));
        } else {
            ImGui::Text("Drop strategy here to start backtest");
        }
        
        // Handle drop
        if (g_drag_state.is_dragging && ImGui::IsMouseReleased(0)) {
            ImVec2 mouse = ImGui::GetMousePos();
            ImVec2 panel_min = ImGui::GetWindowPos();
            ImVec2 panel_max = ImVec2(panel_min.x + ImGui::GetWindowWidth(), 
                                    panel_min.y + ImGui::GetWindowHeight());
            
            if (mouse.x >= panel_min.x && mouse.x <= panel_max.x &&
                mouse.y >= panel_min.y && mouse.y <= panel_max.y) {
                
                std::cout << "DROPPED: " << g_drag_state.strategy_name << std::endl;
                std::string plugin_path = "/Users/rigved/Programs/Night Garden/build/plugins/" + g_drag_state.strategy_name;
                
                PluginHandle handle = LoadStrategyPlugin(plugin_path);
                if (handle.strategy) {
                    std::unordered_map<std::string, std::string> params;
                    params["fast_period"] = "10";
                    params["slow_period"] = "50";
                    handle.strategy->Initialise(params);
                    
                    BacktestEngine engine(*bars_ptr, 10000.0);
                    engine.LoadStrategy(handle.strategy);
                    engine.Run();
                    
                    StrategyResult result;
                    result.name = g_drag_state.strategy_name;
                    result.plugin = handle;
                    result.equity_curve = engine.GetEquityCurve();
                    loaded_strategies.push_back(result);
                    
                    std::cout << "Backtest complete, equity points: " << result.equity_curve.size() << std::endl;
                }
            }
            g_drag_state.is_dragging = false;
        }
        
        ImGui::End();
        return;
    }
    
    // Render tiles for each strategy
    float tile_height = total_height / num_strategies;
    
    for (int i = 0; i < num_strategies; i++) {
        
        ImGui::SetNextWindowPos(ImVec2(left_panel_width, 20 + candlestick_height + i * tile_height)); // OFFSET HERE
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - left_panel_width, tile_height));
        
        std::string window_name = loaded_strategies[i].name + "##" + std::to_string(i);
        ImGui::Begin(window_name.c_str(), nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        
        ImGui::Text("Strategy: %s", loaded_strategies[i].name.c_str());
        
        if (!loaded_strategies[i].equity_curve.empty()) {
            ImGui::PlotLines(
                "Equity",
                loaded_strategies[i].equity_curve.data(),
                loaded_strategies[i].equity_curve.size(),
                0, nullptr, FLT_MAX, FLT_MAX,
                ImVec2(0, tile_height - 80)
            );
        }
        ImGui::End();
    }
}

void ChartPanel::SetBars(std::vector<Bar>* bars) {
    bars_ptr = bars;
}

void ChartPanel::RenderCandlesticks(float left_panel_width, float window_height) {
    if (!bars_ptr || bars_ptr->empty()) return;
    
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(left_panel_width, 20));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - left_panel_width, window_height));
    ImGui::Begin("Market Data", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    if (canvas_size.x <= 0 || canvas_size.y <= 0) {
        ImGui::End();
        return;
    }
    
    // Find price range
    float price_min = (*bars_ptr)[0].low;
    float price_max = (*bars_ptr)[0].high;
    for (const auto& bar : *bars_ptr) {
        if (bar.low < price_min) price_min = bar.low;
        if (bar.high > price_max) price_max = bar.high;
    }
    float price_range = price_max - price_min;
    if (price_range == 0) price_range = 1.0f;
    
    // Draw candlesticks
    int num_bars = bars_ptr->size();
    float x_step = canvas_size.x / num_bars;
    
    for (int i = 0; i < num_bars; i++) {
        const Bar& bar = (*bars_ptr)[i];
        
        float x = canvas_pos.x + (i + 0.5f) * x_step;
        
        // Scale to canvas (flip Y axis)
        auto scale_y = [&](float price) {
            return canvas_pos.y + canvas_size.y * (1.0f - (price - price_min) / price_range);
        };
        
        float y_high = scale_y(bar.high);
        float y_low = scale_y(bar.low);
        float y_open = scale_y(bar.open);
        float y_close = scale_y(bar.close);
        
        // Wick (high-low line)
        draw_list->AddLine(ImVec2(x, y_low), ImVec2(x, y_high), IM_COL32(150, 150, 150, 255), 1.0f);
        
        // Body
        bool bullish = bar.close >= bar.open;
        ImU32 color = bullish ? IM_COL32(0, 200, 0, 255) : IM_COL32(200, 0, 0, 255);
        
        float body_width = x_step * 0.6f;
        float y_top = bullish ? y_close : y_open;
        float y_bottom = bullish ? y_open : y_close;
        
        draw_list->AddRectFilled(
            ImVec2(x - body_width/2, y_top),
            ImVec2(x + body_width/2, y_bottom),
            color
        );
    }
    
    ImGui::End();
}