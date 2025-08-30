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

// @TODO: for keybind
struct KeybindState {

};

namespace ImGuiCustom {
    PressedKeyData* hotkey(const char* label, int* key, float samelineOffset = 0.0f);
    KeybindState get_keybind_state(std::string id);
    void clear_keybind_state();
    inline std::unordered_map<std::string, KeybindState> keybind_state; 
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