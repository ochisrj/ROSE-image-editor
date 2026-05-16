#pragma once

#include "filters/Filter.h"
#include "image/PixelBuffer.h"

class BlurFilter : public Filter {
private:
    float radius;

public:
    BlurFilter();
    ~BlurFilter() override;

    void apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) override;

    void setRadius(float r) { radius = r; }
    float getRadius() const { return radius; }
};
