#pragma once

#include "Layer.h"
#include <string>

namespace rendering {
    class Framebuffer;
}

namespace layers {

class TextLayer : public Layer {
private:
    std::string text;
    float fontSize;
    float color[4];
    float position[2];

public:
    TextLayer(const std::string& name, int id);
    ~TextLayer() override = default;

    void render(rendering::Framebuffer* framebuffer) override;
    std::unique_ptr<Layer> clone() const override;

    void setText(const std::string& t);
    const std::string& getText() const;

    void setFontSize(float size);
    float getFontSize() const;

    void setColor(float r, float g, float b, float a);
    const float* getColor() const;

    void setPosition(float x, float y);
    const float* getPosition() const;
};

} // namespace layers
