#include "LayerManager.h"
#include "rendering/Framebuffer.h"
#include <stdexcept>
#include <algorithm>

namespace layers {

LayerManager::LayerManager() : nextId(0) {
}

void LayerManager::addLayer(std::unique_ptr<Layer> layer) {
    layers.push_back(std::move(layer));
}

void LayerManager::removeLayer(size_t index) {
    if (index >= layers.size()) {
        throw std::out_of_range("Layer index out of range");
    }
    layers.erase(layers.begin() + index);
}

Layer* LayerManager::getLayer(size_t index) {
    if (index >= layers.size()) {
        throw std::out_of_range("Layer index out of range");
    }
    return layers[index].get();
}

const Layer* LayerManager::getLayer(size_t index) const {
    if (index >= layers.size()) {
        throw std::out_of_range("Layer index out of range");
    }
    return layers[index].get();
}

size_t LayerManager::getLayerCount() const {
    return layers.size();
}

void LayerManager::moveLayerUp(size_t index) {
    if (index == 0 || index >= layers.size()) {
        return;
    }
    std::swap(layers[index], layers[index - 1]);
}

void LayerManager::moveLayerDown(size_t index) {
    if (index >= layers.size() - 1) {
        return;
    }
    std::swap(layers[index], layers[index + 1]);
}

std::vector<Layer*> LayerManager::getVisibleLayers() {
    std::vector<Layer*> result;
    for (auto& layer : layers) {
        if (layer->isVisible()) {
            result.push_back(layer.get());
        }
    }
    return result;
}

void LayerManager::compositeToFramebuffer(rendering::Framebuffer* framebuffer) {
    for (auto& layer : layers) {
        if (layer->isVisible()) {
            layer->render(framebuffer);
        }
    }
}

int LayerManager::generateId() {
    return nextId++;
}

} // namespace layers
