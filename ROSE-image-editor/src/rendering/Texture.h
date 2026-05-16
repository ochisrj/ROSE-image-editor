#pragma once

#include <cstdint>

class Texture {
public:
    Texture();
    ~Texture();

    bool loadFromFile(const char* filepath);
    bool createFromData(const unsigned char* data, int width, int height, int channels = 4);
    void destroy();

    void bind(uint32_t slot = 0) const;
    void unbind() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    GLuint getID() const { return textureID; }

private:
    GLuint textureID = 0;
    int width = 0;
    int height = 0;
    int nrChannels = 0;
};
