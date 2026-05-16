#pragma once

#include "tools/Tool.h"
#include "utils/MathUtils.h"

class MoveTool : public Tool {
private:
    int selectedLayerIndex;
    Vec2 dragOffset;

public:
    MoveTool();
    ~MoveTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setSelectedLayerIndex(int index) { selectedLayerIndex = index; }
    int getSelectedLayerIndex() const { return selectedLayerIndex; }
    void setDragOffset(const Vec2& offset) { dragOffset = offset; }
    const Vec2& getDragOffset() const { return dragOffset; }
};
