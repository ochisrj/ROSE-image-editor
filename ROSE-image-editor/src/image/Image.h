#pragma once

#include "PixelBuffer.h"
#include <string>

namespace image {

class Image {
private:
    PixelBuffer buffer;
    std::string filename;
    bool dirty;

public:
    Image();
    ~Image();

    bool loadFromFile(const std::string& path);
    bool saveToFile(const std::string& path);
    bool createBlank(int w, int h);

    int getWidth() const;
    int getHeight() const;
    PixelBuffer* getBuffer();
    const PixelBuffer* getBuffer() const;

    const std::string& getFilename() const { return filename; }
    bool isDirty() const { return dirty; }
    void setDirty(bool d) { dirty = d; }
};

}
