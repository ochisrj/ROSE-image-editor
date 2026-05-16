#pragma once

#include "tools/Tool.h"
#include "utils/MathUtils.h"
#include <cstdint>

enum class GradientType {
    Linear,
    Radial
};

class GradientTool : public Tool {
private:
    GradientType gradientType;
    uint32_t startColor;
    uint32_t endColor;
    Vec2 startPoint;
    Vec2 endPoint;

public:
    GradientTool();
    ~GradientTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setGradientType(GradientType type) { gradientType = type; }
    GradientType getGradientType() const { return gradientType; }
    void setStartColor(uint32_t color) { startColor = color; }
    uint32_t getStartColor() const { return startColor; }
    void setEndColor(uint32_t color) { endColor = color; }
    uint32_t getEndColor() const { return endColor; }
    void setStartPoint(const Vec2& point) { startPoint = point; }
    const Vec2& getStartPoint() const { return startPoint; }
    void setEndPoint(const Vec2& point) { endPoint = point; }
    const Vec2& getEndPoint() const { return endPoint; }
};
