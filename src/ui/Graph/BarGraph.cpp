#include "imgui"

void RenderCandlesticks(float left_panel_width, float window_height) {
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