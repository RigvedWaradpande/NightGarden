#include "ChartPanel.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>

void ChartPanel::SetBars(std::vector<Bar>* bars_ptr) {
    this->bars = bars_ptr;
}

void ChartPanel::RenderMarketData(const std::vector<Bar>& bars) {
    RenderCandlesticks(bars);
}

void ChartPanel::RenderCandlesticks(const std::vector<Bar>& bars) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    if (bars.empty() || canvas_size.x <= 0 || canvas_size.y <= 0) {
        ImGui::Text("No market data available");
        return;
    }
    
    // Find min/max prices for scaling
    float max_price = bars[0].high;
    float min_price = bars[0].low;
    for (const auto& bar : bars) {
        if (bar.high > max_price) max_price = bar.high;
        if (bar.low < min_price) min_price = bar.low;
    }
    float price_range = max_price - min_price;
    if (price_range < 0.01f) price_range = 1.0f;
    
    // Draw background
    draw_list->AddRectFilled(canvas_pos, 
                            ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                            IM_COL32(20, 20, 20, 255));
    
    // Draw candlesticks
    float candle_width = canvas_size.x / bars.size();
    for (size_t i = 0; i < bars.size(); i++) {
        float x = canvas_pos.x + i * candle_width;
        float open_y = canvas_pos.y + canvas_size.y - ((bars[i].open - min_price) / price_range) * canvas_size.y;
        float close_y = canvas_pos.y + canvas_size.y - ((bars[i].close - min_price) / price_range) * canvas_size.y;
        float high_y = canvas_pos.y + canvas_size.y - ((bars[i].high - min_price) / price_range) * canvas_size.y;
        float low_y = canvas_pos.y + canvas_size.y - ((bars[i].low - min_price) / price_range) * canvas_size.y;
        
        bool is_green = bars[i].close >= bars[i].open;
        ImU32 color = is_green ? IM_COL32(0, 200, 0, 255) : IM_COL32(200, 0, 0, 255);
        
        // Draw high-low line
        draw_list->AddLine(ImVec2(x + candle_width/2, high_y), 
                          ImVec2(x + candle_width/2, low_y), 
                          color, 1.0f);
        
        // Draw body
        float body_top = is_green ? close_y : open_y;
        float body_bottom = is_green ? open_y : close_y;
        draw_list->AddRectFilled(ImVec2(x, body_top), 
                                ImVec2(x + candle_width - 1, body_bottom), 
                                color);
    }
    
    // Labels
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + 5, canvas_pos.y + 5));
    ImGui::Text("High: %.2f", max_price);
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + 5, canvas_pos.y + canvas_size.y - 20));
    ImGui::Text("Low: %.2f", min_price);
    
    ImGui::Dummy(canvas_size);
}

void ChartPanel::RenderStrategyTab(size_t strategy_index) {
    if (strategy_index >= loaded_strategies.size()) return;
    
    auto& strategy = loaded_strategies[strategy_index];
    
    // Toggle button
    if (ImGui::Button(strategy.show_graph ? "Show Metrics" : "Show Graph")) {
        strategy.show_graph = !strategy.show_graph;
    }
    
    ImGui::Separator();
    
    if (strategy.show_graph) {
        RenderEquityCurve(strategy.equity_curve);
    } else {
        RenderMetricsView(strategy.metrics);
    }
}

void ChartPanel::RenderEquityCurve(const std::vector<float>& equity) {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    if (equity.empty() || canvas_size.x <= 0 || canvas_size.y <= 0) {
        ImGui::Text("No equity data to display");
        return;
    }
    
    // Find min/max for scaling
    float max_val = *std::max_element(equity.begin(), equity.end());
    float min_val = *std::min_element(equity.begin(), equity.end());
    float range = max_val - min_val;
    if (range < 0.01f) range = 1.0f;
    
    // Draw background
    draw_list->AddRectFilled(canvas_pos, 
                            ImVec2(canvas_pos.x + canvas_size.x, canvas_pos.y + canvas_size.y),
                            IM_COL32(30, 30, 30, 255));
    
    // Draw starting capital line
    float start_val = equity[0];
    float start_y = canvas_pos.y + canvas_size.y - ((start_val - min_val) / range) * canvas_size.y;
    draw_list->AddLine(ImVec2(canvas_pos.x, start_y),
                      ImVec2(canvas_pos.x + canvas_size.x, start_y),
                      IM_COL32(100, 100, 100, 128), 1.0f);
    
    // Draw equity curve
    for (size_t i = 1; i < equity.size(); i++) {
        float x1 = canvas_pos.x + ((i - 1) / (float)(equity.size() - 1)) * canvas_size.x;
        float y1 = canvas_pos.y + canvas_size.y - ((equity[i-1] - min_val) / range) * canvas_size.y;
        float x2 = canvas_pos.x + (i / (float)(equity.size() - 1)) * canvas_size.x;
        float y2 = canvas_pos.y + canvas_size.y - ((equity[i] - min_val) / range) * canvas_size.y;
        
        ImU32 color = (equity[i] >= start_val) ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
        draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, 2.0f);
    }
    
    // Draw axis labels
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + 5, canvas_pos.y + 5));
    ImGui::Text("Max: $%.2f", max_val);
    ImGui::SetCursorScreenPos(ImVec2(canvas_pos.x + 5, canvas_pos.y + canvas_size.y - 20));
    ImGui::Text("Min: $%.2f", min_val);
    
    ImGui::Dummy(canvas_size);
}

void ChartPanel::RenderMetricsView(const PerformanceMetrics& metrics) {
    ImGui::BeginChild("MetricsTable", ImVec2(0, 0), false);
    
    ImGui::Columns(2, "metrics_columns");
    ImGui::Separator();
    
    ImGui::Text("Metric"); ImGui::NextColumn();
    ImGui::Text("Value"); ImGui::NextColumn();
    ImGui::Separator();
    
    // Total Return
    ImGui::Text("Total Return"); ImGui::NextColumn();
    ImU32 return_color = (metrics.total_return >= 0) ? IM_COL32(0, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
    ImGui::TextColored(ImColor(return_color), "%.2f%%", metrics.total_return * 100);
    ImGui::NextColumn();
    
    // Sharpe Ratio
    ImGui::Text("Sharpe Ratio"); ImGui::NextColumn();
    ImGui::Text("%.2f", metrics.sharpe_ratio); ImGui::NextColumn();
    
    // Max Drawdown
    ImGui::Text("Max Drawdown"); ImGui::NextColumn();
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%.2f%%", metrics.max_drawdown * 100);
    ImGui::NextColumn();
    
    // Win Rate
    ImGui::Text("Win Rate"); ImGui::NextColumn();
    ImGui::Text("%.1f%%", metrics.win_rate * 100); ImGui::NextColumn();
    
    // Number of Trades
    ImGui::Text("Total Trades"); ImGui::NextColumn();
    ImGui::Text("%d", metrics.num_trades); ImGui::NextColumn();
    
    ImGui::Separator();
    ImGui::Columns(1);
    
    ImGui::EndChild();
}

void ChartPanel::AddStrategy(const std::string& name, 
                             const std::vector<float>& equity,
                             const std::vector<Trade>& trades) {
    LoadedStrategy strategy;
    strategy.name = name;
    strategy.equity_curve = equity;
    strategy.trades = trades;
    strategy.show_graph = true;
    
    // Calculate metrics
    if (!equity.empty()) {
        strategy.metrics.total_return = (equity.back() - equity.front()) / equity.front();
        
        // Calculate max drawdown
        float peak = equity[0];
        float max_dd = 0.0f;
        for (float val : equity) {
            if (val > peak) peak = val;
            float dd = (peak - val) / peak;
            if (dd > max_dd) max_dd = dd;
        }
        strategy.metrics.max_drawdown = max_dd;
        
        // TODO: Calculate Sharpe ratio (needs returns variance)
        strategy.metrics.sharpe_ratio = 0.0f;
        
        // Calculate win rate
        if (!trades.empty()) {
            int wins = 0;
            for (const auto& trade : trades) {
                if (trade.pnl > 0) wins++;
            }
            strategy.metrics.win_rate = (float)wins / trades.size();
        } else {
            strategy.metrics.win_rate = 0.0f;
        }
        
        strategy.metrics.num_trades = trades.size();
    }
    
    loaded_strategies.push_back(strategy);
}

void ChartPanel::RemoveStrategy(size_t index) {
    if (index < loaded_strategies.size()) {
        loaded_strategies.erase(loaded_strategies.begin() + index);
    }
}