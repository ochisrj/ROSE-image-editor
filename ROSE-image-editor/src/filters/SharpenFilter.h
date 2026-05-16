#pragma once

#include "filters/Filter.h"
#include "image/PixelBuffer.h"

class SharpenFilter : public Filter {
private:
    float amount;

public:
    SharpenFilter();
    ~SharpenFilter() override;

    void apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) override;

    void setAmount(float a) { amount = a; }
    float getAmount() const { return amount; }
};
