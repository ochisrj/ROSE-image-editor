#include "PixelBuffer.h"
#include <cstring>
#include <cstdlib>

namespace image {

PixelBuffer::PixelBuffer() : width(0), height(0), channels(0), data(nullptr) {}

PixelBuffer::~PixelBuffer() {
    destroy();
}

bool PixelBuffer::create(int w, int h, int ch) {
    destroy();
    if (w <= 0 || h <= 0 || ch <= 0) return false;
    data = new (std::nothrow) unsigned char[w * h * ch];
    if (!data) return false;
    std::memset(data, 0, static_cast<size_t>(w * h * ch));
    width = w;
    height = h;
    channels = ch;
    return true;
}

void PixelBuffer::destroy() {
    if (data) {
        delete[] data;
        data = nullptr;
    }
    width = 0;
    height = 0;
    channels = 0;
}

unsigned char* PixelBuffer::getPixel(int x, int y) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) return nullptr;
    return &data[(y * width + x) * channels];
}

const unsigned char* PixelBuffer::getPixel(int x, int y) const {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) return nullptr;
    return &data[(y * width + x) * channels];
}

void PixelBuffer::setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    if (!data || x < 0 || y < 0 || x >= width || y >= height) return;
    unsigned char* pixel = &data[(y * width + x) * channels];
    pixel[0] = r;
    if (channels > 1) pixel[1] = g;
    if (channels > 2) pixel[2] = b;
    if (channels > 3) pixel[3] = a;
}

unsigned char* PixelBuffer::getData() {
    return data;
}

const unsigned char* PixelBuffer::getData() const {
    return data;
}

int PixelBuffer::getSize() const {
    return width * height * channels;
}

PixelBuffer* PixelBuffer::clone() const {
    PixelBuffer* copy = new PixelBuffer();
    if (data && width > 0 && height > 0 && channels > 0) {
        copy->create(width, height, channels);
        std::memcpy(copy->getData(), data, static_cast<size_t>(width * height * channels));
    }
    return copy;
}

}
