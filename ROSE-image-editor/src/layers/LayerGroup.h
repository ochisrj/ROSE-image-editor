#pragma once

#include "Layer.h"
#include <vector>
#include <memory>

namespace rendering {
    class Framebuffer;
}

namespace layers {

class LayerGroup : public Layer {
private:
    std::vector<std::unique_ptr<Layer>> children;

public:
    LayerGroup(const std::string& name, int id);
    ~LayerGroup() override = default;

    void render(rendering::Framebuffer* framebuffer) override;
    std::unique_ptr<Layer> clone() const override;

    void addLayer(std::unique_ptr<Layer> layer);
    void removeLayer(size_t index);
    Layer* getChild(size_t index);
    const Layer* getChild(size_t index) const;
    size_t getChildCount() const;
};

} // namespace layers
