#pragma once

#include "Texture.h"

class Framebuffer {
public:
    Framebuffer();
    ~Framebuffer();

    bool create(int width, int height);
    void destroy();

    void bind() const;
    void unbind() const;

    void resize(int width, int height);

    const Texture& getColorAttachment() const { return colorAttachment; }
    const Texture& getDepthAttachment() const { return depthAttachment; }
    GLuint getFBOID() const { return fboID; }

private:
    GLuint fboID = 0;
    Texture colorAttachment;
    Texture depthAttachment;
    int width = 0;
    int height = 0;
};
