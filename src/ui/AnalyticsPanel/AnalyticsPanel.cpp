#include "AnalyticsPanel.h"
#include "imgui.h"

void AnalyticsPanel::Render(float left_panel_width, float bottom_height){
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(left_panel_width, io.DisplaySize.y - bottom_height));
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x - left_panel_width, bottom_height));
    ImGui::Begin("Analytics", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
    // TODO: Metrics, trade list, logs
    ImGui::End();
}