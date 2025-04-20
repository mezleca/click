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
#include <unordered_set>
#include <iterator>
#include <utility>

enum KeyList {
    INVALID = -1,
    NOT_SET,
#ifdef __linux__
    LEFT,
    MIDDLE,
    RIGHT,
    SCROLLUP,
    SCROLLDOWN,
    PSCROLL_LEFT,
    PSCROLL_DOWN,
    MOUSE4,
    MOUSE5,
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
    MAX_VALUE = MOUSE5
};

struct KeyData {
    KeyList trigger = KeyList::MOUSE5;
    KeyList target = KeyList::LEFT;
    int cps = 12;
};
