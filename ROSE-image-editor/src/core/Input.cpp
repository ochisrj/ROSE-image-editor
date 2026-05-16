#include "core/Input.h"
#include "core/Window.h"
#include <GLFW/glfw3.h>

bool Input::isKeyPressed(int key, const Window& window) const {
    return glfwGetKey(window.getGLFWWindow(), key) == GLFW_PRESS;
}

bool Input::isMouseButtonPressed(int button, const Window& window) const {
    return glfwGetMouseButton(window.getGLFWWindow(), button) == GLFW_PRESS;
}

void Input::getMousePos(const Window& window, double& x, double& y) const {
    glfwGetCursorPos(window.getGLFWWindow(), &x, &y);
}
