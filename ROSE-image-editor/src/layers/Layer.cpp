#include "Layer.h"
#include <algorithm>

namespace layers {

Layer::Layer(const std::string& name, int id)
    : name(name), visible(true), opacity(1.0f), blendMode(BlendMode::Normal), id(id) {
}

void Layer::setOpacity(float o) {
    opacity = std::clamp(o, 0.0f, 1.0f);
}

} // namespace layers
