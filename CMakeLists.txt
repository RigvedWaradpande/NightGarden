cmake_minimum_required(VERSION 3.10)

project(NightGarden CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(IMGUI_DIR $ENV{HOME}/libs/imgui)
set(GLFW_DIR /opt/homebrew/include)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

set(SOURCES 
    src/main.cpp
    src/ui/MainMenu/MainMenu.cpp
    src/ui/StrategyPanel/StrategyPanel.cpp
    src/ui/ChartPanel/ChartPanel.cpp
    src/ui/AnalyticsPanel/AnalyticsPanel.cpp
    src/engine/CSVLoader.cpp
    src/engine/Portfolio.cpp
    src/engine/BacktestEngine.cpp
    src/engine/PluginLoader.cpp
    src/include/types.h
)

add_executable(nightgarden ${SOURCES} ${IMGUI_SOURCES} ${GLFW_SOURCES})

target_include_directories(nightgarden PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    /opt/homebrew/include
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(nightgarden
    OpenGL::GL
    glfw
)

if(APPLE)
    target_link_libraries(nightgarden
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        /opt/homebrew/lib/libglfw.dylib
    )
endif()


# Build SMA strategy as a shared library plugin
add_library(sma_strategy SHARED
    src/strategies/SMAStrategy.cpp
)

# Make sure it can find the headers
target_include_directories(sma_strategy PRIVATE
    ${CMAKE_SOURCE_DIR}
    ~/libs/imgui  # Or wherever your imgui is
    ${IMGUI_DIR}/imgui.cpp
)

# Output the .so to build/plugins/
set_target_properties(sma_strategy PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)