#pragma once

#include <GLFW/glfw3.h>
#include <string>

class Window {
public:
    Window();
    ~Window();

    bool init(const std::string& title, int width, int height);
    void close();
    bool shouldClose() const;
    int getWidth() const;
    int getHeight() const;
    GLFWwindow* getGLFWWindow() const;

private:
    GLFWwindow* m_Window;
    int m_Width;
    int m_Height;
};
