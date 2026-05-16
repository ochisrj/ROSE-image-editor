#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot3d.h"
#include "stb_image.h"
#include "menubar.h"
#include "imgui_tex_inspect.h"
#include "imgui_tex_inspect_internal.h"

#include <stack>
#include <vector>
#include <sstream>
#include <map> 
#include <math.h>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>



// Shader sources
const char* vertexShaderSource = "#version 330 core\n layout (location = 0) in vec3 aPos;\n void main() { gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }\0";
const char* fragmentShaderSource = "#version 330 core\n out vec4 FragColor;\n void main() { FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f); }\n\0";

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 1024;
    int height = 768;

    GLFWwindow* window = glfwCreateWindow(width, height, "ROSE image editor", NULL, NULL);
    if (window == NULL) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, width, height);

    // --- Shader & Buffer Setup (Same as your code) ---
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLfloat vertices[] = { -0.5f, -0.288f, 0.0f, 0.5f, -0.288f, 0.0f, 0.0f, 0.577f, 0.0f };
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // --- Initialize ImGUI & ImPlot ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImPlot3D::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // เรียกใช้หลังจาก Init ImGui เรียบร้อยแล้ว
    ImGuiTexInspect::Init();
    ImGuiTexInspect::CreateContext();

    // --- State Variables ---
    bool show_plot_2d = false;
    static char formula_buffer[128] = "sin(x)";
    bool show_performance = false;
    float fps_history[120] = { 0 };
    int fps_history_idx = 0;


    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw background triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Track FPS history
        fps_history[fps_history_idx % 120] = ImGui::GetIO().Framerate;
        fps_history_idx++;


        ImGui::Begin("Texture Inspector");
        ImGuiTexInspect::ShowDemoWindow();

        ImGui::End();
        
        // 1. TOP MENU BAR
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "Ctrl+N")) { /* New file */ }
                if (ImGui::MenuItem("Open", "Ctrl+O")) { /* Open file */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Save file */ }
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) { /* Save As */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Export As...")) { /* Export */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) { glfwSetWindowShouldClose(window, true); }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* Undo */ }
                if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* Redo */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "Ctrl+X")) { /* Cut */ }
                if (ImGui::MenuItem("Copy", "Ctrl+C")) { /* Copy */ }
                if (ImGui::MenuItem("Paste", "Ctrl+V")) { /* Paste */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Preferences", "Ctrl+K")) { /* Preferences */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Image"))
            {
                if (ImGui::MenuItem("Adjustments...")) { /* Adjustments */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Canvas Size...", "Ctrl+Alt+C")) { /* Canvas Size */ }
                if (ImGui::MenuItem("Image Size...", "Ctrl+Alt+I")) { /* Image Size */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Rotate Canvas")) { /* Rotate */ }
                if (ImGui::MenuItem("Flip Horizontal")) { /* Flip H */ }
                if (ImGui::MenuItem("Flip Vertical")) { /* Flip V */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Layer"))
            {
                if (ImGui::MenuItem("New Layer...", "Ctrl+Shift+N")) { /* New Layer */ }
                if (ImGui::MenuItem("Duplicate Layer")) { /* Duplicate */ }
                if (ImGui::MenuItem("Delete Layer", "Del")) { /* Delete */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Layer Style...")) { /* Layer Style */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Merge Down", "Ctrl+E")) { /* Merge */ }
                if (ImGui::MenuItem("Flatten Image")) { /* Flatten */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Select"))
            {
                if (ImGui::MenuItem("All", "Ctrl+A")) { /* Select All */ }
                if (ImGui::MenuItem("Deselect", "Ctrl+D")) { /* Deselect */ }
                if (ImGui::MenuItem("Inverse", "Ctrl+Shift+I")) { /* Inverse */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Color Range...")) { /* Color Range */ }
                if (ImGui::MenuItem("Modify >")) { /* Modify */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Filter"))
            {
                if (ImGui::BeginMenu("Blur"))
                {
                    if (ImGui::MenuItem("Gaussian Blur")) { /* Gaussian */ }
                    if (ImGui::MenuItem("Motion Blur")) { /* Motion */ }
                    if (ImGui::MenuItem("Box Blur")) { /* Box */ }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Sharpen"))
                {
                    if (ImGui::MenuItem("Unsharp Mask...")) { /* Unsharp */ }
                    if (ImGui::MenuItem("Smart Sharpen...")) { /* Smart */ }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Distort"))
                {
                    if (ImGui::MenuItem("Liquify...")) { /* Liquify */ }
                    if (ImGui::MenuItem("Wave...")) { /* Wave */ }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Noise"))
                {
                    if (ImGui::MenuItem("Add Noise...")) { /* Add */ }
                    if (ImGui::MenuItem("Despeckle")) { /* Despeckle */ }
                    ImGui::EndMenu();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Last Filter", "Ctrl+F")) { /* Last Filter */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Zoom In", "Ctrl+=")) { /* Zoom In */ }
                if (ImGui::MenuItem("Zoom Out", "Ctrl+-")) { /* Zoom Out */ }
                if (ImGui::MenuItem("Fit on Screen", "Ctrl+0")) { /* Fit */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Rulers", "Ctrl+R")) { /* Rulers */ }
                if (ImGui::MenuItem("Grid", "Ctrl+'")) { /* Grid */ }
                if (ImGui::MenuItem("Snap", "Ctrl+Shift+;")) { /* Snap */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                if (ImGui::MenuItem("Layers Panel", "F7")) { /* Layers */ }
                if (ImGui::MenuItem("Channels")) { /* Channels */ }
                if (ImGui::MenuItem("History")) { /* History */ }
                if (ImGui::MenuItem("Toolbar")) { /* Toolbar */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Performance", NULL, &show_performance)) { /* Performance */ }
                ImGui::Separator();
                if (ImGui::MenuItem("Reset Workspace")) { /* Reset */ }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About ROSE Editor")) { /* About */ }
                if (ImGui::MenuItem("Keyboard Shortcuts", "Ctrl+Alt+K")) { /* Shortcuts */ }
                ImGui::EndMenu();
            }


            ImGui::EndMainMenuBar();
        }

        // 2. PLOT 2D WINDOW (GeoGebra Style)
        if (show_plot_2d)
        {
            ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
            if (ImGui::Begin("2D Function Visualizer", &show_plot_2d))
            {
                ImGui::Text("Enter Function:");
                ImGui::SetNextItemWidth(-1);
                ImGui::InputText("##formula", formula_buffer, sizeof(formula_buffer));
                ImGui::TextDisabled("Try: x, x*x, sin(x), cos(x), tan(x)");

                ImGui::Separator();

                if (ImPlot::BeginPlot("##Graph", ImVec2(-1, -1)))
                {
                    ImPlot::SetupAxes("x", "f(x)");
                    ImPlotRect limits = ImPlot::GetPlotLimits();

                    // Generate points dynamically based on view
                    static double x_data[1000], y_data[1000];
                    double step = (limits.X.Max - limits.X.Min) / 999.0;

                    for (int i = 0; i < 1000; ++i) {
                        double x = limits.X.Min + (i * step);
                        double y = 0;

                        // Simple Parser Logic
                        std::string f = formula_buffer;
                        if (f == "x") y = x;
                        else if (f == "x*x" || f == "x^2") y = x * x;
                        else if (f == "sin(x)") y = sin(x);
                        else if (f == "cos(x)") y = cos(x);
                        else if (f == "tan(x)") y = tan(x);
                        else if (f == "sqrt(x)") y = sqrt(x);
                        else y = 0;

                        x_data[i] = x;
                        y_data[i] = y;
                    }

                    ImPlot::SetNextLineStyle(ImVec4(0, 1, 0.7f, 1), 2.0f);
                    ImPlot::PlotLine("f(x)", x_data, y_data, 1000);
                    ImPlot::EndPlot();
                }
            }
            ImGui::End();
        }

        // 3. PERFORMANCE WINDOW (FPS + Dynamic Plot)
        if (show_performance)
        {
            // Ring-buffer for the last 120 FPS samples
            static float  fps_history[120] = {};
            static int    fps_offset = 0;
            static double fps_refresh_time = 0.0;

            double now = ImGui::GetTime();
            // Sample up to 60 times per second so the graph scrolls smoothly
            if (now - fps_refresh_time >= 1.0 / 60.0)
            {
                fps_history[fps_offset] = ImGui::GetIO().Framerate;
                fps_offset = (fps_offset + 1) % IM_ARRAYSIZE(fps_history);
                fps_refresh_time = now;
            }

            // Ring-buffer for frame time (ms) — same approach
            static float  ms_history[120] = {};
            static int    ms_offset = 0;
            {
                static double ms_refresh_time = 0.0;
                if (now - ms_refresh_time >= 1.0 / 60.0)
                {
                    float framerate = ImGui::GetIO().Framerate;
                    ms_history[ms_offset] = (framerate > 0.f) ? 1000.f / framerate : 0.f;
                    ms_offset = (ms_offset + 1) % IM_ARRAYSIZE(ms_history);
                    ms_refresh_time = now;
                }
            }

            ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.45f);
            ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_Always);

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_AlwaysAutoResize;

            ImGui::Begin("FPS window", &show_performance, flags);

            float currentFPS = ImGui::GetIO().Framerate;
            float currentMS = (currentFPS > 0.f) ? 1000.f / currentFPS : 0.f;

            // ── FPS coloured text ──────────────────────────────────────────────────
            ImVec4 fpsColor = (currentFPS >= 55.f) ? ImVec4(0, 1, 0, 1)
                : (currentFPS >= 30.f) ? ImVec4(1, 1, 0, 1)
                : ImVec4(1, 0, 0, 1);
            ImGui::TextColored(fpsColor, "FPS: %.1f", currentFPS);
            ImGui::SameLine();
            ImGui::TextDisabled("(%.2f ms)", currentMS);

            // ── FPS PlotLines — the graph follows the real framerate ───────────────
            ImGui::SeparatorText("FPS History");
            {
                float max_fps = 0.0f;
                float min_fps = FLT_MAX;
                for (int i = 0; i < 120; i++) {
                    if (fps_history[i] > max_fps) max_fps = fps_history[i];
                    if (fps_history[i] < min_fps) min_fps = fps_history[i];
                }

                char overlay[32];
                snprintf(overlay, sizeof(overlay), "%.0f fps", currentFPS);
                ImGui::PlotLines("##fps",
                    fps_history, IM_ARRAYSIZE(fps_history), fps_offset,
                    overlay,
                    FLT_MAX, FLT_MAX,      // เปลี่ยนจาก 0.0f, 200.0f เป็น FLT_MAX ทั้งคู่
                    ImVec2(-1, 70));
            }

            // ── Frame-time PlotLines ───────────────────────────────────────────────
            ImGui::SeparatorText("Frame Time (ms)");
            {
                char overlay[32];
                snprintf(overlay, sizeof(overlay), "%.2f ms", currentMS);
                ImGui::PlotLines("##ms",
                    ms_history, IM_ARRAYSIZE(ms_history), ms_offset,
                    overlay,
                    0.0f, 50.0f,           // y-axis: 0 – 50 ms
                    ImVec2(-1, 70));
            }

            // ── Target budget indicator ────────────────────────────────────────────
            ImGui::Spacing();
            ImGui::ProgressBar(currentFPS / 60.f, ImVec2(-1, 8), "");
            ImGui::TextDisabled("Budget vs 60 fps target");

            ImGui::End();


        }

   
        

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot3D::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    ImGuiTexInspect::DestroyContext(g_InspectContext); // ถ้ามีการเก็บ pointer ไว้
    ImGuiTexInspect::Shutdown();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}