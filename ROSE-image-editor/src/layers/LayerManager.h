#pragma once

#include "Layer.h"
#include <vector>
#include <memory>

namespace rendering {
    class Framebuffer;
}

namespace layers {

class LayerManager {
private:
    std::vector<std::unique_ptr<Layer>> layers;
    int nextId;

public:
    LayerManager();

    void addLayer(std::unique_ptr<Layer> layer);
    void removeLayer(size_t index);
    Layer* getLayer(size_t index);
    const Layer* getLayer(size_t index) const;
    size_t getLayerCount() const;

    void moveLayerUp(size_t index);
    void moveLayerDown(size_t index);

    std::vector<Layer*> getVisibleLayers();
    void compositeToFramebuffer(rendering::Framebuffer* framebuffer);

    int generateId();
};

} // namespace layers
