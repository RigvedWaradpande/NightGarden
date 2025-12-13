#include "StrategyPanel.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

void StrategyPanel::Render(DragState& g_drag_state){
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 20)); // Below menu bar
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x * 0.2f, io.DisplaySize.y - 20));
    ImGui::Begin("Strategy", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    // Discover available strategies (do this once, not every frame)
    static std::vector<std::string> strategies = DiscoverStrategies("/Users/rigved/Programs/Night Garden/build/plugins");
    // Display them
    for (const auto& strategy : strategies) {
        ImGui::Selectable(strategy.c_str());
        
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            g_drag_state.is_dragging = true;
            g_drag_state.strategy_name = strategy;
            g_drag_state.drag_pos = ImGui::GetMousePos();
        }
    }
    ImGui::End();
}

// In StrategyPanel or a utility file
std::vector<std::string> StrategyPanel::DiscoverStrategies(const std::string& plugin_dir) {
    std::vector<std::string> strategyList;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(plugin_dir)) {
        if (entry.path().extension() == ".dylib") {
            strategyList.push_back(entry.path().filename());
        }
    }
    return strategyList;
}