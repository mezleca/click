#pragma once

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#endif

#include "../config/config.hpp"
#include "../gui/gui.hpp"
#include "../common.hpp"

// fucking stupid but who cares
inline std::map<int, std::pair<int, int>> AIDS = {
    { 1, { 0x00000002, 0x00000004 } },
    { 2, { 0x00000008, 0x00000010 }},
    { 4, { 0x00000020, 0x00000040 }}
};

namespace Autoclick {
    void update();
}

namespace Input {
    std::string to_string(int vKey);
    bool is_pressing_key(int vKey);
    void initialize();
    void click(int vKey);
    void remove_key_from_list(int vkey);
    inline std::vector<int> keys;
#ifdef __linux__
    inline Display* XDisplay;
#else
    void normal_press(int vKey, bool kb);
    void other_press(WORD xButton);
    LRESULT kbHook(int code, WPARAM w, LPARAM l);
    LRESULT msHook(int code, WPARAM w, LPARAM l);
    inline HHOOK hKeyHook;
    inline HHOOK hMouseHook;
#endif
}