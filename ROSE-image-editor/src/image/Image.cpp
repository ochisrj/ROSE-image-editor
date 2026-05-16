#include "Image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <cstring>

namespace image {

Image::Image() : dirty(false) {}

Image::~Image() {}

bool Image::loadFromFile(const std::string& path) {
    int w, h, ch;
    unsigned char* imgData = stbi_load(path.c_str(), &w, &h, &ch, 0);
    if (!imgData) return false;

    if (!buffer.create(w, h, ch)) {
        stbi_image_free(imgData);
        return false;
    }

    std::memcpy(buffer.getData(), imgData, static_cast<size_t>(w * h * ch));
    stbi_image_free(imgData);

    filename = path;
    dirty = false;
    return true;
}

bool Image::saveToFile(const std::string& path) {
    if (!buffer.getData() || buffer.getWidth() <= 0 || buffer.getHeight() <= 0) return false;

    int result = stbi_write_png(path.c_str(), buffer.getWidth(), buffer.getHeight(),
                                buffer.getChannels(), buffer.getData(),
                                buffer.getWidth() * buffer.getChannels());
    if (result) {
        dirty = false;
    }
    return result != 0;
}

bool Image::createBlank(int w, int h) {
    if (!buffer.create(w, h, 4)) return false;
    filename.clear();
    dirty = false;
    return true;
}

int Image::getWidth() const {
    return buffer.getWidth();
}

int Image::getHeight() const {
    return buffer.getHeight();
}

PixelBuffer* Image::getBuffer() {
    return &buffer;
}

const PixelBuffer* Image::getBuffer() const {
    return &buffer;
}

}
