#pragma once

#include "tools/Tool.h"
#include "utils/MathUtils.h"

enum class SelectionType {
    Rect,
    Ellipse,
    Lasso,
    MagicWand
};

class SelectionTool : public Tool {
private:
    SelectionType selectionType;
    Vec2 startPoint;
    Vec2 endPoint;
    // Stub: selection region data

public:
    SelectionTool();
    ~SelectionTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setSelectionType(SelectionType type) { selectionType = type; }
    SelectionType getSelectionType() const { return selectionType; }
    void setStartPoint(const Vec2& point) { startPoint = point; }
    const Vec2& getStartPoint() const { return startPoint; }
    void setEndPoint(const Vec2& point) { endPoint = point; }
    const Vec2& getEndPoint() const { return endPoint; }
};
