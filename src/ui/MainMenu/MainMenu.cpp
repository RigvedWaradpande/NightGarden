#include "MainMenu.h"
#include "imgui.h"

void MainMenu::Render(){
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load Data")) {
                //TODO
            }
            if (ImGui::MenuItem("Export Results")) { /* TODO */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Strategy")) {
            if (ImGui::MenuItem("Load Plugin")) { /* TODO */ }
            if (ImGui::MenuItem("Reload")) { /* TODO */ }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}