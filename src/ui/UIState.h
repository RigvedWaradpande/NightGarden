#ifndef UISTATE_H
#define UISTATE_H

#include <string>
#include "imgui.h"

struct DragState {
    bool is_dragging;
    std::string strategy_name;
    ImVec2 drag_pos;
};

#endif