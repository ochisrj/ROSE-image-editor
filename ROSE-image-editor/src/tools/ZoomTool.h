#pragma once

#include "tools/Tool.h"

class ZoomTool : public Tool {
private:
    float zoomFactor;

public:
    ZoomTool();
    ~ZoomTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setZoomFactor(float factor) { zoomFactor = factor; }
    float getZoomFactor() const { return zoomFactor; }
    void zoomIn();
    void zoomOut();
};
