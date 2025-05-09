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
    KeyList hotkey(const char* label, KeyList* key, float samelineOffset = 0.0f);
}

namespace Gui {
    bool initialize();
    void update();
    void finish();
    bool is_focused();

    extern GLFWwindow* window;
    extern int current_item;
}
