#include "input.hpp"
#include <map>
#include <thread>

std::string Input::to_string(KeyList vKey) {
    
    if (vKey > KeyList::MAX_VALUE) {
        return "invalid";
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
            return "invalid";
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
        // std::cout << "kb key: " << kb->vkCode << "\n";
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

void Input::normal_click(int vKey) {

    INPUT input[2] = {};
    std::pair<int, int> keys = AIDS[vKey];

    if (!keys.first) {
        return;
    }

    // down
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = keys.first;

    // up
    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = keys.second;
    SendInput(2, input, sizeof(INPUT));
}

void Input::other_click(WORD xButton) {

    INPUT input[2] = {};

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_XDOWN;
    input[0].mi.mouseData = xButton;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_XUP;
    input[1].mi.mouseData = (DWORD)AIDS2[xButton];

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
    XISelectEvents(XDisplay, root_window, &mask, 1);

    XSync(XDisplay, 0);
    free(mask.mask);

    while (!glfwWindowShouldClose(Gui::window)) {
        
        if (XPending(XDisplay) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        // get current event
        XEvent event;
        XNextEvent(XDisplay, &event);

        // @TODO: fix being almost impossible to bind scrollup/down on linux due to gui not updating on time
        if (event.type == GenericEvent && event.xcookie.extension == xi_op_code) {

            if (XGetEventData(XDisplay, &event.xcookie)) {

                XIRawEvent* raw_event = (XIRawEvent*)event.xcookie.data;
                KeyList key_value = (KeyList)raw_event->detail;

                // erm what the sigma
                if (key_value > KeyList::MAX_VALUE) {
                    printf("yep: %i\n", raw_event->detail);
                    continue;
                }
                
                if (raw_event->evtype == XI_RawButtonPress) {
                    keys.push_back(key_value);
                }
                else if (raw_event->evtype == XI_RawButtonRelease) {
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
void Input::click(KeyList vKey) {

    #ifdef __linux__

    if (XDisplay == NULL) {
        printf("failed to get display\n");
        return;
    }

    XTestFakeButtonEvent(XDisplay, (int)vKey, true, CurrentTime);
    XFlush(XDisplay);
    XTestFakeButtonEvent(XDisplay, (int)vKey, false, CurrentTime);
    XFlush(XDisplay);
    #else
    if (vKey <= KeyList::MIDDLE) {
        normal_click(vKey);
    } else {
        other_click(vKey);
    }
    #endif
}

bool Input::is_pressing_key(KeyList vKey) {
#ifdef __linux__
    auto it = std::find(keys.begin(), keys.end(), vKey);
    return it != keys.end();
#else
    return (GetKeyState((int)vKey) & 0x8000) != 0;
#endif
}

void Autoclick::update() {

    using clock = std::chrono::steady_clock;
    using duration = std::chrono::milliseconds;    
    
    auto start = clock::now();

    // only simulate input if the window is not focused
    if (Gui::is_focused()) {
        return;
    }

    // if the user is not pressing anything, sleep for 10ms
    if (config.keys.size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return;
    }

    // loop through all keys and simulate input it needed
    for (const KeyData& key : config.keys) {
        
        if (!Input::is_pressing_key(key.trigger)) {
            continue;
        }

        // make sure target is set
        if (key.target == KeyList::NOT_SET) {
            continue;
        }

        Input::click(key.target);

        auto target_delay = 1000 / key.cps;

        // @TODO: variation percentage
        if (key.randomized) {
            int variation = target_delay * 0.30;
            target_delay += rand() % variation;
        }

        auto elapsed = std::chrono::duration_cast<duration>(clock::now() - start).count();
        int delay = static_cast<int>(target_delay - elapsed);

        std::this_thread::sleep_for(std::chrono::milliseconds(
            static_cast<int>(delay)
        ));
    }
}