#pragma once

#include <cstdint>

class OpenGLContext {
public:
    OpenGLContext() = default;
    ~OpenGLContext() = default;

    bool init();
    void shutdown();

    void swapBuffers();

    void setViewport(int width, int height);
    void setClearColor(float r, float g, float b, float a);
    void clear();

private:
    int viewportWidth = 0;
    int viewportHeight = 0;
};
