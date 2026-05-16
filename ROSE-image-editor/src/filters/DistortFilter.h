#pragma once

#include "filters/Filter.h"
#include "image/PixelBuffer.h"
#include "utils/MathUtils.h"

enum class DistortionType {
    Wave,
    Spherize,
    Pinch
};

class DistortFilter : public Filter {
private:
    DistortionType distortionType;
    float amount;

public:
    DistortFilter();
    ~DistortFilter() override;

    void apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) override;

    void setDistortionType(DistortionType type) { distortionType = type; }
    DistortionType getDistortionType() const { return distortionType; }
    void setAmount(float a) { amount = a; }
    float getAmount() const { return amount; }
};
