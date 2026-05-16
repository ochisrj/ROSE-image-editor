#pragma once

#include "tools/Tool.h"
#include "utils/MathUtils.h"
#include <string>

class TextTool : public Tool {
private:
    std::string textBuffer;
    float fontSize;
    uint32_t color;
    Vec2 position;

public:
    TextTool();
    ~TextTool() override;

    void onMouseDown(float x, float y, int button) override;
    void onMouseMove(float x, float y) override;
    void onMouseUp(float x, float y, int button) override;
    void onRender() override;

    void setText(const std::string& text) { textBuffer = text; }
    const std::string& getText() const { return textBuffer; }
    void setFontSize(float size) { fontSize = size; }
    float getFontSize() const { return fontSize; }
    void setColor(uint32_t newColor) { color = newColor; }
    uint32_t getColor() const { return color; }
    void setPosition(const Vec2& pos) { position = pos; }
    const Vec2& getPosition() const { return position; }
};
