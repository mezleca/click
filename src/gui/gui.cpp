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

    GLFWwindow* window = NULL;
    int width = 800, height = 600, current_item = 0;
    const char* glsl_version = "#version 130";

    bool initialize() {
                        
        const char* window_error;

        if (!glfwInit()) {
            std::cerr << "failed to initialize GLFW\n";
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        window = glfwCreateWindow(width, height, "elterclick", NULL, NULL);

        if (window == nullptr) {
            glfwGetError(&window_error);
            std::cerr << "failed to create window " << window_error <<  " \n";
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window);
        glfwSwapInterval(1);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cerr << "failed to to load glad" << "\n";
            return false;
        }

        printf("renderer: %s\n", glGetString(GL_RENDERER));

        glViewport(0, 0, width, height);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        io.IniFilename = nullptr;
        
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        ImVec4 background = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        glClearColor(background.x, background.y, background.z, background.w);

        return true;
    }

    void create_button() {
        if (ImGui::Button("create")) {
            KeyData new_comb;
            new_comb.trigger = KeyList::NOT_SET;
            new_comb.target = KeyList::NOT_SET;
            config.keys.push_back(new_comb);      
        }
    }

    void update() {

        glfwPollEvents();

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            return;
        }

        if (!is_focused()) {
            glfwSwapBuffers(window);
            return;
        }

        static auto& io = ImGui::GetIO();
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    
        static constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;
        static constexpr ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit;
    
        ImGui::SetNextWindowSize(ImVec2(width, height));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("elterclick", NULL, flags);

        if (ImGui::BeginTabBar("Tabs", flags))
        {
            if (ImGui::BeginTabItem("general")) {

                const auto keys_size = config.keys.size();

                if (keys_size == 0) {
                    ImGui::Text("Press this button to create a new combination");
                    create_button();
                } else {
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
                    
                    ImGui::SameLine();
                    create_button();
                    ImGui::SameLine();
                    if (ImGui::Button("delete")) {                           
                        auto it = config.keys.begin() + current_item;
                        config.keys.erase(it);
        
                        if (current_item > (int)config.keys.size() - 1) {
                            current_item = 0;
                        }
                    }

                    ImGui::Text("cps");

                    ImGui::SliderInt("##cps", &current_key->cps, 0, 50);
                    ImGui::Checkbox("random", &current_key->randomized);

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

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    bool is_focused() {

        if (!window) {
            return false;
        }
        
        return glfwGetWindowAttrib(window, GLFW_FOCUSED);
    }

    void finish() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}
