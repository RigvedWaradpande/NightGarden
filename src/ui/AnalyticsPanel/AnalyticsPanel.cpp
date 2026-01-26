#include "AnalyticsPanel.h"
#include "imgui.h"

void AnalyticsPanel::Render() {
    ImGui::Text("Analytics Panel");
    ImGui::Separator();
    
    // Placeholder for trade log table
    if (ImGui::BeginTable("TradeLog", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Time");
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("Price");
        ImGui::TableSetupColumn("Quantity");
        ImGui::TableSetupColumn("PnL");
        ImGui::TableHeadersRow();
        
        // Placeholder rows
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("0");
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("BUY");
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("2450.00");
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("4");
        ImGui::TableSetColumnIndex(4);
        ImGui::Text("-");
        
        ImGui::EndTable();
    }
}