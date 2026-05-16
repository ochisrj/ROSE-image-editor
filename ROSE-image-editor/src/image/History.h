#pragma once

#include "PixelBuffer.h"
#include <vector>

namespace image {

class History {
private:
    std::vector<PixelBuffer*> states;
    int currentIndex;

public:
    History();
    ~History();

    void push(PixelBuffer* state);
    PixelBuffer* undo();
    PixelBuffer* redo();

    bool canUndo() const;
    bool canRedo() const;

    PixelBuffer* getCurrent();
    const PixelBuffer* getCurrent() const;

    void clear();
};

}
