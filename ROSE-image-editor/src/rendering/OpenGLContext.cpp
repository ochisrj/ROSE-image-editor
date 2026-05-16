#include "OpenGLContext.h"
#include <glad/glad.h>

bool OpenGLContext::init() {
    if (!gladLoadGL()) {
        return false;
    }
    return true;
}

void OpenGLContext::shutdown() {
}

void OpenGLContext::swapBuffers() {
}

void OpenGLContext::setViewport(int width, int height) {
    viewportWidth = width;
    viewportHeight = height;
    glViewport(0, 0, width, height);
}

void OpenGLContext::setClearColor(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
}

void OpenGLContext::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
