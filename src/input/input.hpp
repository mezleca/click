#pragma once

#ifdef __linux__
#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#endif

#include "../config/config.hpp"
#include "../gui/gui.hpp"
#include "../common.hpp"

namespace Autoclick {
    void update();
}

namespace Input {
    void initialize();
    void click(KeyList vKey);
    bool is_pressing_key(KeyList vKey);

    std::string to_string(KeyList vKey);

    extern std::vector<KeyList> keys;
#ifdef __linux__
    extern Display* XDisplay;
#endif
}
