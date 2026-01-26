#define GL_SILENCE_DEPRECATION
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
#include "engine/PluginLoader.h"
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
    glfwSwapInterval(1);

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Load market data
    std::vector<Bar> bars = LoadCSV("/Users/rigved/Programs/Night Garden/data/reliance.csv");
    std::cout << "Loaded " << bars.size() << " bars" << std::endl;

    MainMenu menu;
    StrategyPanel sp;
    ChartPanel cp;
    cp.SetBars(&bars);
    AnalyticsPanel ap;

    // State for parameter window
    static bool show_param_window = false;
    static int selected_strategy_index = -1;
    static int fast_period = 20;
    static int slow_period = 50;
    
    // State for analytics panel
    static bool show_analytics = true;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Handle keyboard shortcuts
        if (ImGui::IsKeyPressed(ImGuiKey_A) && (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))) {
            show_analytics = !show_analytics;
        }

        // Main window
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("NightGarden", nullptr, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_MenuBar);

        // Menu bar
        if (ImGui::BeginMenuBar()) {
            menu.Render();
            ImGui::EndMenuBar();
        }

        // Left sidebar
        float sidebar_width = io.DisplaySize.x * 0.2f;
        ImGui::BeginChild("LeftPanel", ImVec2(sidebar_width, -1), true);
        sp.Render(g_drag_state);
        ImGui::EndChild();

        ImGui::SameLine();

        // Right content area
        float analytics_height = show_analytics ? 200.0f : 0.0f;
        ImGui::BeginGroup();
        
        // Main content area
        ImGui::BeginChild("MainContent", ImVec2(0, -analytics_height - (show_analytics ? 8.0f : 0.0f)), true);

        // Tab bar for strategies
        if (ImGui::BeginTabBar("StrategyTabs", ImGuiTabBarFlags_Reorderable)) {
            
            // Market data tab (always present)
            if (ImGui::BeginTabItem("Market Data")) {
                cp.RenderMarketData(bars);
                ImGui::EndTabItem();
            }
            
            // Strategy tabs (one per loaded strategy)
            for (size_t i = 0; i < cp.loaded_strategies.size(); i++) {
                bool open = true;
                std::string tab_label = cp.loaded_strategies[i].name + "##" + std::to_string(i);
                if (ImGui::BeginTabItem(tab_label.c_str(), &open)) {
                    cp.RenderStrategyTab(i);
                    
                    // Configure button
                    if (ImGui::Button("Configure Parameters")) {
                        show_param_window = true;
                        selected_strategy_index = i;
                    }
                    
                    ImGui::EndTabItem();
                }
                
                // If user closed the tab
                if (!open) {
                    cp.RemoveStrategy(i);
                    if (selected_strategy_index == (int)i) {
                        show_param_window = false;
                        selected_strategy_index = -1;
                    }
                    i--;
                }
            }
            
            // Visual feedback when dragging
            if (g_drag_state.is_dragging) {
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "Drop strategy here to load and backtest");
            }
            
            ImGui::EndTabBar();
        }

        // Drop zone detection
        if (g_drag_state.is_dragging && !ImGui::IsMouseDown(0)) {
            // Mouse released - check if we're over the main content area
            ImVec2 mouse_pos = ImGui::GetMousePos();
            ImVec2 content_pos = ImGui::GetWindowPos();
            ImVec2 content_size = ImGui::GetWindowSize();
            ImVec2 content_min = content_pos;
            ImVec2 content_max = ImVec2(content_min.x + content_size.x, content_min.y + content_size.y);
            
            if (mouse_pos.x >= content_min.x && mouse_pos.x <= content_max.x &&
                mouse_pos.y >= content_min.y && mouse_pos.y <= content_max.y) {
                
                // Dropped in content area - load strategy
                std::string plugin_path = "/Users/rigved/Programs/Night Garden/build/plugins/" + g_drag_state.strategy_name;
                std::cout << "Loading plugin: " << plugin_path << std::endl;
                
                PluginHandle plugin = LoadStrategyPlugin(plugin_path);
                
                // Initialize with default or custom parameters
                std::unordered_map<std::string, std::string> params;
                params["fast_period"] = "20";
                params["slow_period"] = "50";
                plugin.strategy->Initialise(params);

                if (plugin.strategy != nullptr) {
                    std::cout << "Plugin loaded successfully" << std::endl;
                    
                    std::cout << "Creating BacktestEngine..." << std::endl;
                    BacktestEngine engine(bars, 10000.0);
                    
                    std::cout << "Loading strategy..." << std::endl;
                    engine.LoadStrategy(plugin.strategy);
                    
                    std::cout << "Running backtest..." << std::endl;
                    engine.Run();
                    
                    std::cout << "Getting equity curve..." << std::endl;
                    
                    std::vector<float> equity_curve = engine.GetEquityCurve();
                    std::vector<Trade> trades;  // TODO: Get from portfolio
                    
                    cp.AddStrategy(g_drag_state.strategy_name, equity_curve, trades);
                    
                    UnloadStrategyPlugin(plugin);
                    
                    std::cout << "Strategy added to tabs" << std::endl;
                } else {
                    std::cout << "Failed to load plugin" << std::endl;
                }
            }
            
            g_drag_state.is_dragging = false;
        }

        ImGui::EndChild();

        // Bottom analytics panel
        if (show_analytics) {
            ImGui::BeginChild("Analytics", ImVec2(0, 0), true);
            ap.Render();
            ImGui::EndChild();
        }
        
        ImGui::EndGroup();

        ImGui::End();

        // Parameter configuration window
        if (show_param_window && selected_strategy_index >= 0 && selected_strategy_index < (int)cp.loaded_strategies.size()) {
            ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
            ImGui::Begin("Strategy Parameters", &show_param_window);
            
            auto& strategy = cp.loaded_strategies[selected_strategy_index];
            ImGui::Text("Configuring: %s", strategy.name.c_str());
            ImGui::Separator();
            
            ImGui::SliderInt("Fast Period", &fast_period, 5, 100);
            ImGui::SliderInt("Slow Period", &slow_period, 10, 200);
            
            ImGui::Spacing();
            
            if (ImGui::Button("Apply & Rerun", ImVec2(-1, 0))) {
                // TODO: Pass params to strategy and re-run backtest
                std::cout << "Rerunning with fast=" << fast_period << ", slow=" << slow_period << std::endl;
                show_param_window = false;
            }
            
            ImGui::End();
        }

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