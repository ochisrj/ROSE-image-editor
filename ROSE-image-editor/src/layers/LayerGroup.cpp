#include "LayerGroup.h"
#include "rendering/Framebuffer.h"
#include <stdexcept>

namespace layers {

LayerGroup::LayerGroup(const std::string& name, int id)
    : Layer(name, id) {
}

void LayerGroup::render(rendering::Framebuffer* framebuffer) {
    if (!framebuffer) return;
    for (auto& child : children) {
        if (child->isVisible()) {
            child->render(framebuffer);
        }
    }
}

std::unique_ptr<Layer> LayerGroup::clone() const {
    auto cloned = std::make_unique<LayerGroup>(getName(), getId());
    cloned->setVisible(isVisible());
    cloned->setOpacity(getOpacity());
    cloned->setBlendMode(getBlendMode());

    for (const auto& child : children) {
        cloned->children.push_back(child->clone());
    }
    return cloned;
}

void LayerGroup::addLayer(std::unique_ptr<Layer> layer) {
    children.push_back(std::move(layer));
}

void LayerGroup::removeLayer(size_t index) {
    if (index >= children.size()) {
        throw std::out_of_range("Child index out of range");
    }
    children.erase(children.begin() + index);
}

Layer* LayerGroup::getChild(size_t index) {
    if (index >= children.size()) {
        throw std::out_of_range("Child index out of range");
    }
    return children[index].get();
}

const Layer* LayerGroup::getChild(size_t index) const {
    if (index >= children.size()) {
        throw std::out_of_range("Child index out of range");
    }
    return children[index].get();
}

size_t LayerGroup::getChildCount() const {
    return children.size();
}

} // namespace layers
