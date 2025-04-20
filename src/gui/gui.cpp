#include "gui.hpp"

namespace ImGuiCustom {

    // @TODO: "press to change" text on hover
    KeyList hotkey(const char* label, KeyList* key, float samelineOffset) {
   
        KeyList selected_key = KeyList::INVALID;

        ImGui::PushID(label);

        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonActive));
        ImGui::Button(Input::to_string(*key).c_str(), { 150.0f, 0.0f });
        ImGui::PopStyleColor();

        bool hover = ImGui::IsItemHovered();

        // if the user is hovering and pressing a key, update the KeyList values
        if (hover && Input::keys.size() != 0) {
            selected_key = Input::keys.back();
        }

        ImGui::PopID();
        
        return selected_key;
    }
}

namespace Gui {

    SDL_Window* window = NULL;
    bool done = false;
    int width = 800, height = 600, current_item = 0;

    bool initialize() {
                        
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        
        #ifdef __linux__
            const char* glsl_version = "#version 130";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); 
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        #elif _WIN32
            const char*glsl_version = "#version 130";
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0); 
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        #endif

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
            std::cerr << "failed to initialize SDL2" << "\n";
            return false;
        }
    
        window = SDL_CreateWindow(
            "elterclick",   
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );

        SDL_SetWindowMinimumSize(window, 800, 600);

        if (window == nullptr) {
            std::cerr << "failed to cerate window" << "\n";
            SDL_Quit();
            return false;
        }

        SDL_GLContext gl_context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, gl_context);

        if (!gl_context) {
            std::cerr << "failed to create OpenGL context: " << SDL_GetError() << "\n";
            SDL_DestroyWindow(window);
            SDL_Quit();
            return false;
        }

        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            std::cerr << "failed to to load glad" << "\n";
            SDL_GL_DeleteContext(gl_context);
            SDL_Quit();
            return false;
        }

        printf("renderer: %s\n", glGetString(GL_RENDERER));

        // vsync on
        SDL_GL_SetSwapInterval(1);

        glViewport(0, 0, width, height);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.IniFilename = nullptr;
        
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);

        ImVec4 background = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(background.x, background.y, background.z, background.w);

        return true;
    }

    void update() {

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            if (event.type == SDL_QUIT) {
                done = true;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
                done = true;
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window)) {
                SDL_GetWindowSize(window, &width, &height);
            }
        }

        if (!is_focused()) {
            SDL_GL_SwapWindow(window);
            return;
        }

        static auto& io = ImGui::GetIO();
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    
        static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        static constexpr ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit;
    
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("elterclick", NULL, flags);

        if (ImGui::BeginTabBar("Tabs", flags))
        {
            if (ImGui::BeginTabItem("general")) {

                // @TODO: otimize this shit
                // convert KeyData vector to a string vector
                std::vector<std::string> item_strings;
                std::vector<const char*> items;
                item_strings.reserve(config.keys.size());
                items.reserve(config.keys.size());
                KeyData* current_key = &config.keys.at(current_item);

                // surely this wont crash
                for (size_t i = 0; i < config.keys.size(); i++) {
                    item_strings.push_back(Input::to_string(config.keys.at(i).trigger) + " -> " + Input::to_string(config.keys.at(i).target));
                    items.push_back(item_strings.back().c_str());
                }

                ImGui::Text("combinations");
                ImGui::Combo("##combination", &current_item, items.data(), config.keys.size());

                if (ImGui::Button("create")) {
                    
                    bool exists = false;

                    // check if theres already a not set key thing
                    for (size_t i = 0; i < config.keys.size(); i++) {

                        KeyData key = config.keys.at(i);

                        if (key.trigger == KeyList::NOT_SET) {
                            exists = true;
                            break;
                        }
                    }

                    // dont create a new one if theres already a unset keybind
                    if (!exists) {
                        KeyData new_comb;
                        new_comb.trigger = KeyList::NOT_SET;
                        new_comb.target = KeyList::NOT_SET;
                        config.keys.push_back(new_comb);
                    }
                }
                
                ImGui::SameLine();

                if (ImGui::Button("delete")) {

                    if (config.keys.size() != 1) {
                        
                        auto it = config.keys.begin() + current_item;
                        config.keys.erase(it);
        
                        if (current_item > (int)config.keys.size() - 1) {
                            current_item = 0;
                        }
                    }
                }

                ImGui::Text("cps");

                ImGui::SliderInt("##cps", &current_key->cps, 0, 50);
                ImGui::Checkbox("randomizer", &config.randomized);

                ImGui::Text("keybinds");
                if (ImGui::BeginTable("keytable", 2, table_flags, ImVec2(width / 2, 0.0f))) {
                
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);

                    KeyList new_trigger_key = ImGuiCustom::hotkey("trigger", &current_key->trigger);

                    // make sure the trigger is not equal to the target
                    if (new_trigger_key > KeyList::NOT_SET && current_key->target != new_trigger_key) {
                        current_key->trigger = new_trigger_key;
                    }

                    ImGui::TableSetColumnIndex(1);
                    
                    KeyList new_target_key = ImGuiCustom::hotkey("target", &current_key->target);

                    // make sure the target is not equal to the trigger
                    if (new_target_key > KeyList::NOT_SET && current_key->trigger != new_target_key) {
                        current_key->target = new_target_key;
                    }

                    ImGui::EndTable();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("config")) {

                if (ImGui::Button("save")) {
                    Config::save();
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
        ImGui::Render();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // https://gamedev.stackexchange.com/a/210948
    bool is_focused() {

        if (!window) {
            return false;
        }
        
        uint32_t flags = SDL_GetWindowFlags(window);
        return (flags & SDL_WINDOW_INPUT_FOCUS) != 0;
    }

    void finish() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}
