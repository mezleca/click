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
#include <map>
#include <thread>

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
    K_0        = XK_0,          // 0x0030
    K_9        = XK_9,          // 0x0039
    K_A        = XK_A,          // 0x0041
    K_B        = XK_B,
    K_Z        = XK_Z,          // 0x005A
    F2         = XK_F2,
    F12        = XK_F12,        // 0xFFC9
#else
/*              MOUSE STUFF             */
    LEFT         = VK_LBUTTON,    // 0x01
    MIDDLE       = VK_MBUTTON,    // 0x04
    RIGHT        = VK_RBUTTON,    // 0x02
    SCROLLUP     = 0x08,
    SCROLLDOWN   = 0x09,
    PSCROLL_LEFT = 0x0A,
    PSCROLL_DOWN = 0x0B,
    MOUSE4       = VK_XBUTTON1,   // 0x05
    MOUSE5       = VK_XBUTTON2,   // 0x06
/*              KEYBOARD STUFF          */
    K_A = 0x41,
    K_Z = 0x5A,
    F1  = VK_F1,
    F12 = VK_F12,
#endif
    MAX_KB_VALUE = F12,
    MAX_MOUSE_VALUE = MOUSE5
};

enum PressedKeyType {
    MOUSE = 0,
    KEYBOARD
};

struct PressedKeyData {
    int key;
    PressedKeyType type;

    bool operator==(int vKey) const {
        return key == vKey;
    }

    bool operator==(const PressedKeyData& other) const {
        return key == other.key && type == other.type;
    }
};

struct KeyData {
    int trigger = KeyList::MOUSE5;
    int target = KeyList::LEFT;
    int cps = 12;
    bool randomized = false;
    PressedKeyType trigger_type;
    PressedKeyType target_type;
};
