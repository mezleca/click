#include "input.hpp"

namespace Input {

    Display* XDisplay = NULL;
    std::vector<KeyList> keys;

    std::string to_string(KeyList vKey) {
        
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

    void initialize() {
        
        // could pass NULL but yeah
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

        while(!Gui::done) {
            
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

                        auto it = std::find(keys.begin(), keys.end(), key_value);

                        if (it != keys.end()) {
                            int index = std::distance(keys.begin(), it);
                            keys.erase(keys.begin() + index);
                        }
                    }
                }
                
                XFreeEventData(XDisplay, &event.xcookie);
            }
        }

        XCloseDisplay(XDisplay);
    }

    // @TODO: windows
    void click(KeyList vKey) {

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
        // @TODO: making input for different keys on windows in a pain in the ass
        // why tf do i need different flags for different keys???
        #endif
    }

    bool is_pressing_key(KeyList vKey) {
        #ifdef __linux__
        auto it = std::find(keys.begin(), keys.end(), vKey);
        return it != keys.end();
        #else
        return (GetKeyState((int)vKey) & 0x8000) != 0;
        #endif
    }
}

namespace Autoclick {

    void update() {

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
            if (config.randomized) {
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
}
