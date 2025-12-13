#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "ui/MainMenu/MainMenu.h"
#include "ui/StrategyPanel/StrategyPanel.h"
#include "ui/ChartPanel/ChartPanel.h"
#include "ui/AnalyticsPanel/AnalyticsPanel.h"
#include "engine/CSVLoader.h"
#include "engine/BacktestEngine.h"
#include "engine/Portfolio.h"
#include "ui/UIState.h"

DragState g_drag_state = {false, "", ImVec2(0,0)};

int main() {
    // GLFW setup
    if (!glfwInit()) return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "NightGarden - Backtest", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // YOUR CODE STARTS HERE - Dummy data for equity curve
    std::vector<Bar> bars = LoadCSV("/Users/rigved/Programs/Night Garden/data/reliance.csv");
    std::cout << "Loaded " << bars.size() << " bars" << std::endl;

    MainMenu menu;
    StrategyPanel sp;
    ChartPanel cp;
    cp.SetBars(&bars);
    AnalyticsPanel ap;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI CODE HERE
        ImGui::Begin("NightGarden");
        //Main menu bar (very top)
        
        menu.Render();

        // Left sidebar (20% width)
        
        sp.Render(g_drag_state);

        // Main area (scrubber + equity curve)
        float left_panel_width = io.DisplaySize.x * 0.2f;
        float bottom_height = 200.0f;

        cp.Render(left_panel_width, bottom_height, g_drag_state);

        // Bottom panel (analytics + logs)
        
        ap.Render(left_panel_width, bottom_height);

        ImGui::End();
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}