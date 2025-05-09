#include <stdio.h>
#include <string>
#include <chrono>
#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#include "mingw.thread.h"
#else
#include <thread>
#endif
#include "input/input.hpp"
#include "config/config.hpp"
#include "gui/gui.hpp"

int main(int argv, char** args) {

    Config::initialize();

    // make sure we initialize everything properly
    if (!Gui::initialize()) {
        std::cerr << "failed to initialize gui system\n";
        return -1;
    }

    // create a new thread to input event loop
    std::thread input_thread(Input::initialize);
    
    while (!glfwWindowShouldClose(Gui::window)) {
        Autoclick::update();
        Gui::update();
    }

    // close sdl window and shit
    Gui::finish();

    // wait until the input thread is finished
    input_thread.join();
    
    return 0;
}
