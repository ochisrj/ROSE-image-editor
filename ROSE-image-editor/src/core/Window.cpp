#include "core/Window.h"
#include <glad/glad.h>

Window::Window()
    : m_Window(nullptr)
    , m_Width(0)
    , m_Height(0)
{
}

Window::~Window() {
    close();
}

bool Window::init(const std::string& title, int width, int height) {
    m_Width = width;
    m_Height = height;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_Window) {
        return false;
    }

    glfwMakeContextCurrent(m_Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
        return false;
    }

    return true;
}

void Window::close() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

bool Window::shouldClose() const {
    return m_Window ? glfwWindowShouldClose(m_Window) : true;
}

int Window::getWidth() const {
    return m_Width;
}

int Window::getHeight() const {
    return m_Height;
}

GLFWwindow* Window::getGLFWWindow() const {
    return m_Window;
}
