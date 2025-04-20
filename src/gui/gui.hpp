#pragma once

#include <glad/glad.h>
#include <SDL.h>
#include <SDL_events.h>
#include <SDL_opengl.h>
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl2.h>

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

    extern SDL_Window* window;
    extern bool done;
    extern int current_item;
}
