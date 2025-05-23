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

void Input::remove_key_from_list(int vkey) {
    auto it = std::find(keys.begin(), keys.end(), vkey);
    if (it != keys.end()) {
        int index = std::distance(keys.begin(), it);
        keys.erase(keys.begin() + index);
    }
}

#ifdef _WIN32

LRESULT CALLBACK Input::kbHook(int code, WPARAM w, LPARAM l) {
    
    if (code == HC_ACTION) {

        KBDLLHOOKSTRUCT* kb = reinterpret_cast<KBDLLHOOKSTRUCT*>(l);

        if (kb->flags == 0) {
            keys.push_back(kb->vkCode);
        } else {
            remove_key_from_list(kb->vkCode);
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

        switch (w)
        {
            case WM_LBUTTONDOWN:    
                keys.push_back(KeyList::LEFT);
                break; 
            case WM_LBUTTONUP:
                remove_key_from_list(KeyList::LEFT);
                break;
            case WM_RBUTTONDOWN:
                keys.push_back(KeyList::RIGHT);
                break; 
            case WM_RBUTTONUP:
                remove_key_from_list(KeyList::RIGHT);
                break;
            case WM_MBUTTONDOWN:
                keys.push_back(KeyList::MIDDLE);
                break; 
            case WM_MBUTTONUP:
                remove_key_from_list(KeyList::MIDDLE);
                break;
            case WM_XBUTTONDOWN: {
                if (key == XBUTTON1) {
                    keys.push_back(KeyList::MOUSE4);
                } else {               
                    keys.push_back(KeyList::MOUSE5);
                }
                break;
            }
            case WM_XBUTTONUP: {
                if (key == XBUTTON1) {
                    remove_key_from_list(KeyList::MOUSE4);
                } else {               
                    remove_key_from_list(KeyList::MOUSE5);
                }
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
    input[0].mi.mouseData = KeyList::MOUSE4 ? 1 : 2;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_XUP;
    input[1].mi.mouseData = KeyList::MOUSE4 ? 1 : 2;

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

                // since im lazy asf, we cant get the get from KeyList on kb
                XIRawEvent* raw_event = (XIRawEvent*)event.xcookie.data;
                auto key_value = raw_event->detail;

                if (key_value > KeyList::F12 || key_value < KeyList::LEFT) {
                    continue;
                }

                if (raw_event->evtype == XI_RawButtonPress || raw_event->evtype == XI_RawKeyPress) {
                    keys.push_back(key_value);
                }
                else if (raw_event->evtype == XI_RawButtonRelease || raw_event->evtype == XI_RawKeyRelease) {
                    remove_key_from_list(key_value);
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
            other_press(vKey);
        }
    } else {
        normal_press(vKey, true);
    }
    #endif
}

bool Input::is_pressing_key(int vKey) {
    auto it = std::find(keys.begin(), keys.end(), vKey);
    return it != keys.end();
}

void Autoclick::update() {

    using clock = std::chrono::steady_clock;

    // only simulate input if the window is not focused
    // if the user is not pressing anything

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
        
        if (!Input::is_pressing_key(key.trigger)) {
            continue;
        }

        // make sure target is set
        if (key.target == KeyList::NOT_SET) {
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