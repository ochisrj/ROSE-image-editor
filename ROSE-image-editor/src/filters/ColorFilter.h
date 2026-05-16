#pragma once

#include "filters/Filter.h"
#include "image/PixelBuffer.h"
#include "utils/MathUtils.h"

class ColorFilter : public Filter {
private:
    float brightness;
    float contrast;
    float saturation;
    float hue;

public:
    ColorFilter();
    ~ColorFilter() override;

    void apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) override;

    void setBrightness(float b) { brightness = b; }
    float getBrightness() const { return brightness; }
    void setContrast(float c) { contrast = c; }
    float getContrast() const { return contrast; }
    void setSaturation(float s) { saturation = s; }
    float getSaturation() const { return saturation; }
    void setHue(float h) { hue = h; }
    float getHue() const { return hue; }
};
