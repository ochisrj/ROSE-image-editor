#pragma once

#include "tools/Tool.h"
#include "image/PixelBuffer.h"
#include <cstdint>

class BrushTool : public Tool {
private:
    float size;
    float hardness;
    float opacity;
    uint32_t color;

public:
    BrushTool();
    ~BrushTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setSize(float newSize) { size = newSize; }
    float getSize() const { return size; }
    void setHardness(float newHardness) { hardness = newHardness; }
    float getHardness() const { return hardness; }
    void setOpacity(float newOpacity) { opacity = newOpacity; }
    float getOpacity() const { return opacity; }
    void setColor(uint32_t newColor) { color = newColor; }
    uint32_t getColor() const { return color; }
};
