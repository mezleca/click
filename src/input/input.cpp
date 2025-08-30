#include "input.hpp"

std::string get_keyboard_key(int vKey) {
#ifdef __linux__
    KeySym kc = XkbKeycodeToKeysym(Input::XDisplay, vKey, 0, 0);
    std::string result = XKeysymToString(kc);

    if (result != "") {
        return result;
    }
#else
    UINT sc = MapVirtualKeyA(vKey, MAPVK_VK_TO_VSC);
    LONG lparam = sc << 16;

    char buf[64] = {0};
    int len = GetKeyNameTextA(lparam, buf, sizeof(buf));

    if (len > 0) {
        return std::string(buf, len);
    }
#endif
    return "unknown";
}

std::string Input::to_string(int vKey) {
    if (vKey == 0 || vKey > KeyList::MAX_KB_VALUE) {
        return "not set";
    }

    switch (vKey)
    {
        case KeyList::INVALID:
            return "invalid";
        case KeyList::NOT_SET:
            return "not set";
        case KeyList::LEFT:
            return "left";
        case KeyList::RIGHT:
            return "right";
        case KeyList::MIDDLE:
            return "middle";
        case KeyList::SCROLLUP:
            return "scroll up";
        case KeyList::SCROLLDOWN:
            return "scroll down";
        case KeyList::PSCROLL_DOWN:
        case KeyList::PSCROLL_LEFT:
            return "no idea";
        case KeyList::MOUSE4:
            return "mouse4";
        case KeyList::MOUSE5:
            return "mouse5";
        default:
            return get_keyboard_key(vKey);
    }
}

void Input::remove_key(PressedKeyData key_data) {
    auto it = std::find_if(keys.begin(), keys.end(), [&key_data](const PressedKeyData& pressed) {
        return pressed.key == key_data.key && pressed.type == key_data.type;
    });
    
    if (it != keys.end()) {
        keys.erase(it);
    }
}

#ifdef _WIN32

LRESULT CALLBACK Input::kbHook(int code, WPARAM w, LPARAM l) {
    if (code == HC_ACTION) {
        PressedKeyData pressed_key;
        KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(l);

        pressed_key.key = kb->vkCode;
        pressed_key.type = PressedKeyType::KEYBOARD;

        if (w == WM_KEYDOWN || w == WM_SYSKEYDOWN) {
            // prevent duplcated keys
            auto existing = std::find_if(keys.begin(), keys.end(), [&pressed_key](const PressedKeyData& pressed) {
                return pressed.key == pressed_key.key && pressed.type == pressed_key.type;
            });
            
            if (existing == keys.end()) {
                keys.push_back(pressed_key);
            }
        } else if (w == WM_KEYUP || w == WM_SYSKEYUP) {
            remove_key(pressed_key);
        }

        // std::cout << "kb key: " << kb->vkCode << " flags: " << kb->flags << " scode: " << kb->scanCode <<"\n";
    }

    return CallNextHookEx(hKeyHook, code, w, l);
}

LRESULT CALLBACK Input::msHook(int code, WPARAM w, LPARAM l) {
    if (code == HC_ACTION) {
        MSLLHOOKSTRUCT* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(l);

        if (w == WM_MOUSEMOVE) {
            return CallNextHookEx(hMouseHook, code, w, l);
        }

        UINT key = HIWORD(ms->mouseData);
        PressedKeyData pressed_key;
        pressed_key.type = PressedKeyType::MOUSE;

        switch (w)
        {
            case WM_LBUTTONDOWN:
                pressed_key.key = KeyList::LEFT;
                keys.push_back(pressed_key);
                break; 
            case WM_LBUTTONUP:
                pressed_key.key = KeyList::LEFT;
                remove_key(pressed_key);
                break;
            case WM_RBUTTONDOWN:
                pressed_key.key = KeyList::RIGHT;
                keys.push_back(pressed_key);
                break; 
            case WM_RBUTTONUP:
                pressed_key.key = KeyList::RIGHT;
                remove_key(pressed_key);
                break;
            case WM_MBUTTONDOWN:
                pressed_key.key = KeyList::MIDDLE;
                keys.push_back(pressed_key);
                break; 
            case WM_MBUTTONUP:
                pressed_key.key = KeyList::MIDDLE;
                remove_key(pressed_key);
                break;
            case WM_XBUTTONDOWN: {
                if (key == XBUTTON1) {
                    pressed_key.key = KeyList::MOUSE4;
                } else {               
                    pressed_key.key = KeyList::MOUSE5;
                }
                keys.push_back(pressed_key);
                break;
            }
            case WM_XBUTTONUP: {
                if (key == XBUTTON1) {
                    pressed_key.key = KeyList::MOUSE4;
                } else {               
                    pressed_key.key = KeyList::MOUSE5;
                }
                remove_key(pressed_key);
                break;
            }
        }
    }

    return CallNextHookEx(hMouseHook, code, w, l);
}

constexpr std::array<std::pair<DWORD, DWORD>, 3> MOUSE_FLAGS = {{
    { MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP },
    { MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP },
    { MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP }
}};

void Input::normal_press(int vKey, bool isKb) {
    INPUT input[2] = {};

    if (isKb) {

        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = static_cast<WORD>(vKey);
        input[0].ki.dwFlags = 0;

        input[1].type = INPUT_KEYBOARD; 
        input[1].ki.wVk = static_cast<WORD>(vKey);
        input[1].ki.dwFlags = KEYEVENTF_KEYUP;

    } else {
        // invalid
        if (vKey < 1 || vKey > 3) {
            return;
        }

        input[0].type = INPUT_MOUSE;
        input[0].mi.dwFlags = MOUSE_FLAGS[vKey-1].first;

        input[1].type = INPUT_MOUSE;
        input[1].mi.dwFlags = MOUSE_FLAGS[vKey-1].second;
    }

    SendInput(2, input, sizeof(INPUT));
}

void Input::other_press(WORD xButton) {
    INPUT input[2] = {};

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
    input[0].mi.mouseData = xButton;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_XUP;
    input[1].mi.mouseData = xButton;

    // send up/down keypress
    SendInput(2, input, sizeof(INPUT));
}
#endif

void Input::initialize() {
#ifdef __linux__
    std::string name = std::getenv("DISPLAY");
    XDisplay = XOpenDisplay(name.c_str());

    if (XDisplay == NULL) {
        printf("failed to get display\n");
        return;
    }

    int event_code, error, xi_op_code;

    if (!XQueryExtension(XDisplay, "XInputExtension", &xi_op_code, &event_code, &error)) {
        fprintf(stderr, "xinput not available\n");
        return;
    }

    // capture from the root window instead of a normal window
    Window root_window = DefaultRootWindow(XDisplay);

    // create mask buffer
    XIEventMask mask;

    // set the mask shit
    mask.deviceid = XIAllMasterDevices;
    mask.mask_len = XIMaskLen(XI_LASTEVENT);
    mask.mask = static_cast<unsigned char *>(calloc(mask.mask_len, sizeof(char)));

    // apply mask and select events
    XISetMask(mask.mask, XI_RawButtonPress);
    XISetMask(mask.mask, XI_RawButtonRelease);
    XISetMask(mask.mask, XI_RawKeyPress);
    XISetMask(mask.mask, XI_RawKeyRelease);
    XISelectEvents(XDisplay, root_window, &mask, 1);

    XSync(XDisplay, 0);
    free(mask.mask);

    while (!glfwWindowShouldClose(Gui::window)) {
        if (XPending(XDisplay) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        // get current event
        XEvent event;
        XNextEvent(XDisplay, &event);

        // @TODO: fix being almost impossible to bind scrollup/down on linux due to gui not updating on time
        if (event.type == GenericEvent && event.xcookie.extension == xi_op_code) {
            if (XGetEventData(XDisplay, &event.xcookie)) {
                // create new keydata
                PressedKeyData key_data;
                
                XIRawEvent* raw_event = (XIRawEvent*)event.xcookie.data;
                int key_code = raw_event->detail;

                // debug
                // std::cout << "ev_type: " << raw_event->evtype  << " | button: " << key_code << "\n";

                if (raw_event->evtype == XI_RawButtonPress || raw_event->evtype == XI_RawButtonRelease) {
                    // accept only known mouse button range
                    if (key_code < KeyList::LEFT || key_code > KeyList::MAX_MOUSE_VALUE) {
                        // unknown/ignored button
                    } else {
                        key_data.key = key_code;
                        key_data.type = PressedKeyType::MOUSE;
                        if (raw_event->evtype == XI_RawButtonPress) {
                            keys.push_back(key_data);
                        } else {
                            remove_key(key_data);
                        }
                    }
                }
                else if (raw_event->evtype == XI_RawKeyPress || raw_event->evtype == XI_RawKeyRelease) {
                    if (key_code != NoSymbol) {
                        key_data.key = key_code;
                        key_data.type = PressedKeyType::KEYBOARD;
                        if (raw_event->evtype == XI_RawKeyPress) {
                            keys.push_back(key_data);
                        } else {
                            remove_key(key_data);
                        }
                    }
                }
            }
            
            XFreeEventData(XDisplay, &event.xcookie);
        }
    }

    XCloseDisplay(XDisplay);
#else 
    HINSTANCE hInst = GetModuleHandleW(nullptr);

    hKeyHook = SetWindowsHookExW(WH_KEYBOARD_LL, kbHook, hInst, 0);
    hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, msHook, hInst, 0);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hKeyHook);
    UnhookWindowsHookEx(hMouseHook);
#endif
}

// @TODO: windows
void Input::click(int vKey) {
    #ifdef __linux__
    if (XDisplay == NULL) {
        printf("failed to get display\n");
        return;
    }
    
    if (vKey <= KeyList::MAX_MOUSE_VALUE) {
        XTestFakeButtonEvent(XDisplay, vKey, true, CurrentTime);
        XFlush(XDisplay);
        XTestFakeButtonEvent(XDisplay, vKey, false, CurrentTime);
        XFlush(XDisplay);
    } else {
        XTestFakeKeyEvent(XDisplay, vKey, true, CurrentTime);
        XFlush(XDisplay);
        XTestFakeKeyEvent(XDisplay, vKey, false, CurrentTime);
        XFlush(XDisplay);
    }
    #else
    if (vKey <= KeyList::MAX_MOUSE_VALUE) {
        if (vKey <= KeyList::MIDDLE) {
            normal_press(vKey, false);
        } else {
            WORD xbtn = (vKey == KeyList::MOUSE4) ? XBUTTON1 : XBUTTON2;
            other_press(xbtn);
        }
    } else {
        normal_press(vKey, true);
    }
    #endif
}

PressedKeyData* Input::pressed_key(int vKey, PressedKeyType keyType) {
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        if (it->key == vKey && it->type == keyType) {
            return &(*it);
        }
    }
    return nullptr;
}

void Autoclick::update() {
    using clock = std::chrono::steady_clock;

    // only simulate input if the window is not focused
    if (Gui::is_focused()) {
        return;
    }

    if (config.keys.size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return;
    }

    // loop through all keys and simulate input it needed
    for (const KeyData& key : config.keys) {
        auto start = std::chrono::steady_clock::now();
        auto pressed_key = Input::pressed_key(key.trigger, key.trigger_type); 
        
        // check if the key is valid
        if (pressed_key == nullptr) {
            continue;
        }

        // make sure target is set
        if (key.target == KeyList::NOT_SET) {
            continue;
        }

        // check if the key type matches
        if (key.trigger_type != pressed_key->type) {
            continue;
        }

        auto target_delay = 1000 / key.cps;

        // @TODO: variation percentage
        if (key.randomized) {
            int variation = target_delay * 0.50;
            int random_adj = (rand() % (2 * variation + 1)) - variation;

            target_delay += random_adj;

            if (target_delay < 1) {
                target_delay = 1;
            }
        }

        Input::click(key.target);

        int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count();
        int delay = static_cast<int>(target_delay - elapsed);
        
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
    }
}