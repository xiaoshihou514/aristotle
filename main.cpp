#include <GLFW/glfw3.h>
#include <cstdlib>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <stdio.h>
#include <string>

#include "actions.h"

static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char **) {
    // init glfw
    glfwSetErrorCallback(glfw_error_callback);
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

    // Our state
    char *const text_area_buf = (char *const)malloc(1024 * 1024 * sizeof(char));
    const size_t buf_size = 1024 * 1024;
    std::string curr_file = "New Proof.ndp";
    bool open = true;

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
                open_new_file(curr_file);
            }
            if (ImGui::MenuItem("Save")) {
                save_file();
            }
            // I'm sure Intellij users will feel right at home
            if (ImGui::MenuItem("Compile")) {
                compile_file();
            }
            if (ImGui::MenuItem("Check")) {
                check_file();
            }
            if (ImGui::MenuItem("Format")) {
                format_file();
            }
            ImGui::EndMenuBar();
        }

        // imgui won't let you input more characters if buf is full :)
        ImGui::InputTextMultiline("##UniqueIdentifer", text_area_buf, buf_size,
                                  ImVec2(winsize.x - 15, winsize.y - 95));
        ImGui::Separator();
        ImGui::Text("Editing %s", curr_file.c_str());

        ImGui::ShowDemoWindow();

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
