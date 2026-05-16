#pragma once

#include "tools/Tool.h"
#include "utils/MathUtils.h"

class CropTool : public Tool {
private:
    Rect cropRect;

public:
    CropTool();
    ~CropTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setCropRect(const Rect& rect) { cropRect = rect; }
    const Rect& getCropRect() const { return cropRect; }
    void applyCrop();
};
