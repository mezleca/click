#pragma once

#include <glad/glad.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_DEFINE_MATH_OPERATORS
#include "GLFW/glfw3.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include "../config/config.hpp"
#include "../common.hpp"

namespace ImGuiCustom {
    int hotkey(const char* label, int* key, float samelineOffset = 0.0f);
}

namespace Gui {
    bool initialize();
    bool is_focused();
    void update();
    void finish();
    void create_button();

    inline GLFWwindow* window;
    inline int width = 800, height = 600, current_item = 0;
}
