#include "Framebuffer.h"
#include <glad/glad.h>
#include <cassert>

Framebuffer::Framebuffer() = default;

Framebuffer::~Framebuffer() {
    destroy();
}

bool Framebuffer::create(int w, int h) {
    destroy();

    width = w;
    height = h;

    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    unsigned char* blankData = new unsigned char[width * height * 4];
    for (int i = 0; i < width * height * 4; i += 4) {
        blankData[i] = 0;
        blankData[i + 1] = 0;
        blankData[i + 2] = 0;
        blankData[i + 3] = 255;
    }
    colorAttachment.createFromData(blankData, width, height, 4);
    delete[] blankData;

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment.getID(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::destroy() {
    if (fboID != 0) {
        glDeleteFramebuffers(1, &fboID);
        fboID = 0;
    }
    colorAttachment.destroy();
    depthAttachment.destroy();
    width = 0;
    height = 0;
}

void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glViewport(0, 0, width, height);
}

void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int w, int h) {
    width = w;
    height = h;

    unsigned char* blankData = new unsigned char[width * height * 4];
    for (int i = 0; i < width * height * 4; i += 4) {
        blankData[i] = 0;
        blankData[i + 1] = 0;
        blankData[i + 2] = 0;
        blankData[i + 3] = 255;
    }
    colorAttachment.createFromData(blankData, width, height, 4);
    delete[] blankData;

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment.getID(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
