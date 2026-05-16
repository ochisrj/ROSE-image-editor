#pragma once

namespace image {

class PixelBuffer {
private:
    int width;
    int height;
    int channels;
    unsigned char* data;

public:
    PixelBuffer();
    ~PixelBuffer();

    PixelBuffer(const PixelBuffer&) = delete;
    PixelBuffer& operator=(const PixelBuffer&) = delete;

    bool create(int w, int h, int ch);
    void destroy();

    unsigned char* getPixel(int x, int y);
    const unsigned char* getPixel(int x, int y) const;
    void setPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    unsigned char* getData();
    const unsigned char* getData() const;
    int getSize() const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getChannels() const { return channels; }

    PixelBuffer* clone() const;
};

}
