#pragma once

#include <string>
#include <memory>

namespace rendering {
    class Framebuffer;
}

namespace layers {

enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Overlay
};

class Layer {
private:
    std::string name;
    bool visible;
    float opacity;
    BlendMode blendMode;
    int id;

public:
    Layer(const std::string& name, int id);
    virtual ~Layer() = default;

    virtual void render(rendering::Framebuffer* framebuffer) = 0;
    virtual std::unique_ptr<Layer> clone() const = 0;

    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }

    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }

    float getOpacity() const { return opacity; }
    void setOpacity(float o);

    BlendMode getBlendMode() const { return blendMode; }
    void setBlendMode(BlendMode mode) { blendMode = mode; }

    int getId() const { return id; }
    void setId(int i) { id = i; }
};

} // namespace layers
