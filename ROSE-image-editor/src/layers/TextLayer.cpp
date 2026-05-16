#include "TextLayer.h"

namespace layers {

TextLayer::TextLayer(const std::string& name, int id)
    : Layer(name, id), text(""), fontSize(12.0f) {
    color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
    position[0] = 0.0f; position[1] = 0.0f;
}

void TextLayer::render(rendering::Framebuffer* framebuffer) {
    // Stub: Text rendering to be implemented
}

std::unique_ptr<Layer> TextLayer::clone() const {
    auto cloned = std::make_unique<TextLayer>(getName(), getId());
    cloned->setVisible(isVisible());
    cloned->setOpacity(getOpacity());
    cloned->setBlendMode(getBlendMode());
    cloned->text = text;
    cloned->fontSize = fontSize;
    for (int i = 0; i < 4; ++i) cloned->color[i] = color[i];
    for (int i = 0; i < 2; ++i) cloned->position[i] = position[i];
    return cloned;
}

void TextLayer::setText(const std::string& t) { text = t; }
const std::string& TextLayer::getText() const { return text; }

void TextLayer::setFontSize(float size) { fontSize = size; }
float TextLayer::getFontSize() const { return fontSize; }

void TextLayer::setColor(float r, float g, float b, float a) {
    color[0] = r; color[1] = g; color[2] = b; color[3] = a;
}
const float* TextLayer::getColor() const { return color; }

void TextLayer::setPosition(float x, float y) { position[0] = x; position[1] = y; }
const float* TextLayer::getPosition() const { return position; }

} // namespace layers
