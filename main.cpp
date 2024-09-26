#include <GLFW/glfw3.h>
#include <cstdio>
#include <imgui.h>
#include <imfilebrowser.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

// Our state
static char *const text_area_buf =
    (char *const)malloc(1024 * 1024 * sizeof(char));
static const size_t buf_size = 1024 * 1024;
static std::string curr_file = "New Proof.ndp";
static std::string error_msg = "";
static bool modified = false;
static std::string modified_status = "";
static ImGui::FileBrowser fileDialog = ImGui::FileBrowser(
    ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc |
    ImGuiFileBrowserFlags_ConfirmOnEnter |
    ImGuiFileBrowserFlags_EditPathString);

static int setmodified(ImGuiInputTextCallbackData *data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
        modified = true;
    }
    return 0;
}

int main(int, char **) {
    // init glfw
    glfwSetErrorCallback([](auto error, auto desc) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
    });
    if (!glfwInit()) {
        return 1;
    }
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow *window =
        glfwCreateWindow(1280, 720, "Aristotle", nullptr, nullptr);
    if (window == nullptr) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    style.ItemSpacing.x = 16;
    style.FramePadding.y = 10;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.Fonts->AddFontFromFileTTF("JetBrainsMono-Regular.ttf", 24.0f);
    io.Fonts->Build();

    // init state
    bool init = true;
    bool open = true; // always true
    fileDialog.SetTitle("Choose proof file");
    fileDialog.SetTypeFilters({".ndp"});

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Just use the main window
        auto winsize = io.DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(winsize);
        ImGui::Begin("Root window", &open,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_MenuBar);

        // draw menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("Open")) {
                fileDialog.Open();
            }
            if (ImGui::MenuItem("Save")) {
                auto f = fopen(curr_file.c_str(), "w");
                if (f != NULL && fputs(text_area_buf, f) >= 0) {
                    modified = false;
                } else {
                    error_msg.assign("Failed to write to ");
                    error_msg.append(curr_file);
                }
            }
            // I'm sure Intellij users will feel right at home
            if (ImGui::MenuItem("Compile")) {
                std::cout << "Compile" << std::endl;
            }
            if (ImGui::MenuItem("Check")) {
                std::cout << "Check" << std::endl;
            }
            if (ImGui::MenuItem("Format")) {
                std::cout << "Format" << std::endl;
            }
            ImGui::EndMenuBar();
        }

        // set focus on text area on startup
        if (init) {
            ImGui::SetKeyboardFocusHere(0);
            init = false;
        }
        // imgui won't let you input more characters if buf is full :)
        ImGui::InputTextMultiline("##UniqueIdentifer", text_area_buf, buf_size,
                                  ImVec2(winsize.x - 15, winsize.y - 95),
                                  ImGuiInputTextFlags_CallbackEdit,
                                  setmodified);
        ImGui::Separator();
        if (!error_msg.empty()) {
            ImGui::Text("%s", error_msg.c_str());
        } else {
            if (modified) {
                modified_status.assign(" [modified]");
            } else {
                modified_status.assign("");
            }
            ImGui::Text("Editing %s%s", curr_file.c_str(),
                        modified_status.c_str());
        }

        // file picker logic
        fileDialog.Display();
        if (fileDialog.HasSelected()) {
            curr_file.assign(fileDialog.GetSelected().string());
            fileDialog.ClearSelected();
            auto f = fopen(curr_file.c_str(), "r");

            if (f != NULL && std::filesystem::file_size(curr_file) < buf_size &&
                fread(text_area_buf, sizeof(char), buf_size, f) > 0) {
            } else {
                error_msg.assign("Failed to open file ");
                error_msg.append(curr_file);
            }
            fclose(f);
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    free(text_area_buf);

    // Tell others to clean up
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
