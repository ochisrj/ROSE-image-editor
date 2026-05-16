#include "AdjustmentLayer.h"
#include "rendering/Framebuffer.h"

namespace layers {

AdjustmentLayer::AdjustmentLayer(const std::string& name, int id)
    : Layer(name, id), adjustmentType(AdjustmentType::Brightness), intensity(0.0f) {
}

void AdjustmentLayer::render(rendering::Framebuffer* framebuffer) {
    if (!framebuffer) return;

    switch (adjustmentType) {
        case AdjustmentType::Brightness:
            framebuffer->adjustBrightness(intensity);
            break;
        case AdjustmentType::Contrast:
            framebuffer->adjustContrast(intensity);
            break;
        case AdjustmentType::Hue:
            framebuffer->adjustHue(intensity);
            break;
    }
}

std::unique_ptr<Layer> AdjustmentLayer::clone() const {
    auto cloned = std::make_unique<AdjustmentLayer>(getName(), getId());
    cloned->setVisible(isVisible());
    cloned->setOpacity(getOpacity());
    cloned->setBlendMode(getBlendMode());
    cloned->adjustmentType = adjustmentType;
    cloned->intensity = intensity;
    return cloned;
}

void AdjustmentLayer::setAdjustmentType(AdjustmentType type) { adjustmentType = type; }
AdjustmentType AdjustmentLayer::getAdjustmentType() const { return adjustmentType; }

void AdjustmentLayer::setIntensity(float value) { intensity = value; }
float AdjustmentLayer::getIntensity() const { return intensity; }

} // namespace layers
