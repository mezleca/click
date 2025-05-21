#pragma once
#ifdef _WIN32
#include <windows.h> 
#endif
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <string>
#include <string.h>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>
#include <unordered_map>

enum KeyList {
    INVALID = -1,
    NOT_SET,
#ifdef __linux__
/*              MOUSE STUFF             */
    LEFT,
    MIDDLE,
    RIGHT,
    SCROLLUP,
    SCROLLDOWN,
    PSCROLL_LEFT,
    PSCROLL_DOWN,
    MOUSE4,
    MOUSE5,
/*              KEYBOARD STUFF          */
    K_SPACE    = XK_space,      // 0x0020
    K_EXCLAM   = XK_exclam,     // 0x0021
    K_QUOTEDBL = XK_quotedbl,   // 0x0022
    K_0        = XK_0,          // 0x0030
    K_1        = XK_1,
    K_9        = XK_9,          // 0x0039
    K_A        = XK_A,          // 0x0041
    K_B        = XK_B,
    K_Z        = XK_Z,          // 0x005A
    K_a        = XK_a,          // 0x0061
    K_z        = XK_z,          // 0x007A
    F1         = XK_F1,         // 0xFFBE
    F2         = XK_F2,
    F12        = XK_F12,        // 0xFFC9
#else
    LEFT         = VK_LBUTTON,    // 0x01
    MIDDLE       = VK_MBUTTON,    // 0x04
    RIGHT        = VK_RBUTTON,    // 0x02
    SCROLLUP     = 0x08,
    SCROLLDOWN   = 0x09,
    PSCROLL_LEFT = 0x0A,
    PSCROLL_DOWN = 0x0B,
    MOUSE4       = VK_XBUTTON1,   // 0x05
    MOUSE5       = VK_XBUTTON2,    // 0x06
#endif
    MAX_KB_VALUE = F12,
    MAX_MOUSE_VALUE = MOUSE5
};

struct KeyData {
    int trigger = KeyList::MOUSE5;
    int target = KeyList::LEFT;
    int cps = 12;
    bool randomized = false;
};
