#pragma once

#include "tools/Tool.h"
#include "image/PixelBuffer.h"

class EraserTool : public Tool {
private:
    float size;
    float hardness;

public:
    EraserTool();
    ~EraserTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setSize(float newSize) { size = newSize; }
    float getSize() const { return size; }
    void setHardness(float newHardness) { hardness = newHardness; }
    float getHardness() const { return hardness; }
};
