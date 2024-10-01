#include <GLFW/glfw3.h>
#include <chrono>
#include <imgui.h>
#include <imfilebrowser.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <cstdio>
#include <tuple>

#define MSG_DURATION 1000000000

#ifdef __APPLE__
#define GLSL_VERSION "#version 150"
#else
#define GLSL_VERSION "#version 130"
#endif

#ifdef _WIN_32
// the worst platform
#define NDPC_COMMAND "ndpc.exe"
#else
#define NDPC_COMMAND "ndpc"
#endif

// Our state
static const size_t buf_len = 1024 * 1024;
static char *const text_area_buf = (char *const)malloc(buf_len * sizeof(char));
static std::string curr_file = "New Proof.ndp";
static std::string msg = "";
static auto msg_set = std::chrono::system_clock::now();
static std::string error_msg = "";
static bool modified = false;
static bool maybe_show_error_msg = false;
static std::tuple<std::string, int> cmd_result = std::tuple("", 0);
static std::string modified_status = "";
static ImGui::FileBrowser fileDialog = ImGui::FileBrowser(
    ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CloseOnEsc |
    ImGuiFileBrowserFlags_ConfirmOnEnter |
    ImGuiFileBrowserFlags_EditPathString);

static void save_file() {
    auto f = fopen(curr_file.c_str(), "w");

    // make file unix compiliant
    auto str = std::string(text_area_buf);
    if (str.back() != '\n') {
        str.push_back('\n');
    }
    if (f != NULL && fputs(str.c_str(), f) >= 0) {
        modified = false;
        msg.assign("File saved");
        msg_set = std::chrono::system_clock::now();
    } else {
        error_msg.assign("Failed to write to ");
        error_msg.append(curr_file);
    }
    fclose(f);
}

static void load_file() {
    auto f = fopen(curr_file.c_str(), "r");

    if (f != NULL && std::filesystem::file_size(curr_file) < buf_len &&
        fread(text_area_buf, sizeof(char), buf_len, f) > 0) {
    } else {
        error_msg.assign("Failed to open file ");
        error_msg.append(curr_file);
    }
    fclose(f);
}

static void exec(std::string cmd) {
    std::array<char, 128> buffer;
    std::string result;
    // our app is single threaded so it's fine
    // ...probably
    std::unique_ptr<FILE, void (*)(FILE *)> pipe(
        popen(cmd.c_str(), "r"), [](auto f) {
            int code = pclose(f);
            std::get<1>(cmd_result) = code;
        });
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
           nullptr) {
        result += buffer.data();
    }
    std::get<0>(cmd_result).assign(result);
}

static int last_exit_code() { return std::get<1>(cmd_result); }

static void remove_ansi_escape(std::string input, std::string &output) {
    bool parsing_esc = false;

    for (std::string::iterator it = input.begin(); it < input.end(); it++) {
        if (*it == 27) {
            parsing_esc = true;
            continue;
        }
        if (parsing_esc == true) {
            if (*it == 'm') {
                parsing_esc = false;
            }
            continue;
        }
        output.push_back(*it);
    }
}

int main(int argc, char **argv) {
    // init glfw
    glfwSetErrorCallback([](auto error, auto desc) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, desc);
    });
    if (!glfwInit()) {
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

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
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    std::string dir =
        weakly_canonical(std::filesystem::path(argv[0])).parent_path().u8string();
    std::string font_path = dir;
    font_path.append("/JetBrainsMono-Regular.ttf");
    io.Fonts->AddFontFromFileTTF(font_path.c_str(), 24.0f);
    io.Fonts->Build();

    // init state
    bool init = true;
    bool open = true; // always true
    fileDialog.SetTitle("Choose proof file");
    fileDialog.SetTypeFilters({".ndp"});
    auto exepath = dir.append("/").append(NDPC_COMMAND);

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

        // Munu bar logic
        if (ImGui::BeginMenuBar()) {
            if (ImGui::MenuItem("Open") ||
                ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O)) {
                fileDialog.Open();
            }
            if (ImGui::MenuItem("Save") ||
                ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S)) {
                save_file();
            }
            // I'm sure Intellij users will feel right at home
            // TODO: the hotkeys sometimes won't work
            if (ImGui::MenuItem("Compile") ||
                ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_C)) {
                save_file();
                std::string cmd = exepath;
                cmd.append(" \"").append(curr_file).append("\" 2>&1");
                exec(cmd);
                maybe_show_error_msg = true;
                if (last_exit_code() == 0) {
                    msg.assign("Compilation was successful!");
                    msg_set = std::chrono::system_clock::now();
                }
            }
            if (ImGui::MenuItem("Check") ||
                ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_C)) {
                save_file();
                std::string cmd = exepath;
                cmd.append(" check \"").append(curr_file).append("\" 2>&1");
                exec(cmd);
                maybe_show_error_msg = true;
                if (last_exit_code() == 0) {
                    msg.assign("Proof is correct!");
                    msg_set = std::chrono::system_clock::now();
                }
            }
            if (ImGui::MenuItem("Format") ||
                ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiMod_Shift |
                                ImGuiKey_F)) {
                std::string cmd = exepath;
                cmd.append(" format --apply \"")
                    .append(curr_file)
                    .append("\" 2>&1");
                exec(cmd);
                if (std::get<1>(cmd_result) == 0) {
                    // success, reload
                    load_file();
                } else {
                    // failed, rewrite
                    save_file();
                }
                maybe_show_error_msg = true;
                if (last_exit_code() == 0) {
                    msg.assign("Successfully formatted!");
                    msg_set = std::chrono::system_clock::now();
                }
            }
            ImGui::EndMenuBar();
        }

        // Main UI logic

        // set focus on text area on startup
        if (init) {
            ImGui::SetKeyboardFocusHere(0);
            init = false;
        }
        // imgui won't let you input more characters if buf is full :)
        ImGui::InputTextMultiline(
            "##UniqueIdentifer", text_area_buf, buf_len,
            ImVec2(winsize.x - 15, winsize.y - 95),
            ImGuiInputTextFlags_CallbackEdit, [](auto data) {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackEdit) {
                    // catch: if modified is not static it would need a capture
                    // and won't work
                    modified = true;
                }
                return 0;
            });
        ImGui::Separator();

        // status line
        if (!error_msg.empty()) {
            ImGui::Text("%s", error_msg.c_str());
        } else if (!msg.empty() &&
                   (std::chrono::system_clock::now() - msg_set).count() <
                       MSG_DURATION) {
            ImGui::Text("%s", msg.c_str());
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
            load_file();
        }

        // message popup
        if (maybe_show_error_msg && std::get<1>(cmd_result) != 0) {
            ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("Something went wrong...")) {
                std::string temp = "";
                remove_ansi_escape(std::get<0>(cmd_result), temp);
                ImGui::Text("%s", temp.c_str());
                ImGui::Separator();
                if (ImGui::Button("Close")) {
                    maybe_show_error_msg = false;
                }
                ImGui::End();
            }
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
