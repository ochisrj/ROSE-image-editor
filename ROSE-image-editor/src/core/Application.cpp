#include "core/Application.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

Application::Application()
    : m_ImGuiLayer(nullptr)
    , m_MenuBar(nullptr)
    , m_DocumentManager(nullptr)
    , m_Running(false)
{
}

Application::~Application() {
    Shutdown();
}

void Application::Run() {
    init();
    m_Running = true;

    while (m_Running && !m_Window.shouldClose()) {
        update();
        render();
    }
}

void Application::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_Window.close();
    m_Running = false;
}

void Application::init() {
    if (!glfwInit()) {
        return;
    }

    if (!m_Window.init(m_Config.getWindowTitle(), m_Config.getWindowWidth(), m_Config.getWindowHeight())) {
        glfwTerminate();
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(m_Window.getGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Application::update() {
    glfwPollEvents();
}

void Application::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_Window.getGLFWWindow());
}
