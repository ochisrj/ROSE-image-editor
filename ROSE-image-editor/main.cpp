#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "implot.h"
#include "implot3d.h"
//#include "stb_image.h"
#include "cascadiafont.h"
#include "menubar.h"
#include "camera.h"
#include "workspace.h"
#include "PreferencesWindow.h"
#include "LanguageTest.h"



#include <stack>
#include <vector>
#include <sstream>
#include <map> 
#include <math.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <cmath>

// Shader sources
const char* vertexShaderSource = "#version 330 core\n layout (location = 0) in vec3 aPos;\n void main() { gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }\0";
const char* fragmentShaderSource = "#version 330 core\n out vec4 FragColor;\n void main() { FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f); }\n\0";

// Keep glViewport in sync with the actual (framebuffer) window size so the
// background triangle always fills the whole window and stays centered.
static void FramebufferSizeCallback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}



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
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glViewport(0, 0, width, height);

    // --- Shader & Buffer Setup (direct-to-backbuffer background triangle) ---
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

    // ---- Thai-capable font setup (global) ----
    // CascadiaCode has NO Thai glyphs. Old code AddFontFromMemoryTTF(cascadia, GetGlyphRangesThai())
    // requested Thai but rendered tofu. Fix: load Cascadia for Latin, then MergeMode
    // RD CHULAJARUEK.ttf for U+0E00..U+0E7F so EVERY ImGui window (menus, panels, inputs) can show Thai.
    ImFont* mainFont = nullptr;
    ImFont* thaiFont = nullptr;
    {
        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = false;
        mainFont = io.Fonts->AddFontFromMemoryTTF(cascadia, cascadiasize, 16.0f, &cfg, io.Fonts->GetGlyphRangesDefault());
        if (mainFont)
        {
            ImFontConfig thaiCfg;
            thaiCfg.MergeMode = true;
            thaiCfg.PixelSnapH = true;
            // RD CHULAJARUEK.ttf is the Thai font (77848 bytes). Also try ZF/GoogleSans fallback.
            const char* candidates[] = {
                "RD CHULAJARUEK.ttf",
                "./RD CHULAJARUEK.ttf",
                "ROSE-image-editor/RD CHULAJARUEK.ttf",
                "C:\\Users\\ochig\\Project\\Imgui\\ROSE-image-editor\\ROSE-image-editor\\RD CHULAJARUEK.ttf",
                "GoogleSans.ttf",
                "./GoogleSans.ttf",
                "ZF#2ndPixelus.ttf",
            };
            for (const char* p : candidates)
            {
                thaiFont = io.Fonts->AddFontFromFileTTF(p, 16.0f, &thaiCfg, io.Fonts->GetGlyphRangesThai());
                if (thaiFont) break;
            }
            // Fallback: standalone Thai font (for LanguageTest grid even if merge failed)
            if (!thaiFont)
            {
                ImFontConfig standaloneCfg;
                for (const char* p : candidates)
                {
                    thaiFont = io.Fonts->AddFontFromFileTTF(p, 16.0f, &standaloneCfg, io.Fonts->GetGlyphRangesThai());
                    if (thaiFont) break;
                }
            }
            // Make merged font the default for the whole app
            io.FontDefault = mainFont;
        }
    }
    LanguageTest::SetFonts(mainFont, thaiFont ? thaiFont : mainFont);
    // Global scale (affects every window, not just LanguageTest). Keep 1.0 = 16px.
    // LanguageTest slider will modify io.FontGlobalScale / Style.FontScaleMain live.

    // Init dedicated Preferences window (Photoshop-style scaffold)
    PreferencesWindow::Init();
    
    // Main while loop
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw the background triangle directly into the window framebuffer.
        // glViewport is kept up to date by FramebufferSizeCallback on resize,
        // so the triangle always fills the window and stays centered.
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // --- Start ImGui Frame ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        MenuBar::Draw(window);


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    Workspace::Shutdown();
    ImGui_ImplOpenGL3_Shutdown();   
    ImGui_ImplGlfw_Shutdown();
    ImPlot3D::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}