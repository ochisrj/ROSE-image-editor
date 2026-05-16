#pragma once

#include "Layer.h"
#include <memory>

namespace rendering {
    class Framebuffer;
}

namespace layers {

enum class AdjustmentType {
    Brightness,
    Contrast,
    Hue
};

class AdjustmentLayer : public Layer {
private:
    AdjustmentType adjustmentType;
    float intensity;

public:
    AdjustmentLayer(const std::string& name, int id);
    ~AdjustmentLayer() override = default;

    void render(rendering::Framebuffer* framebuffer) override;
    std::unique_ptr<Layer> clone() const override;

    void setAdjustmentType(AdjustmentType type);
    AdjustmentType getAdjustmentType() const;

    void setIntensity(float value);
    float getIntensity() const;
};

} // namespace layers
