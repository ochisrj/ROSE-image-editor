#pragma once

#include <string>
#include <unordered_map>
#include "image/PixelBuffer.h"

enum class FilterCategory {
    Blur,
    Sharpen,
    Distort,
    Color,
    Noise,
    Other
};

class Filter {
protected:
    std::string name;
    FilterCategory category;

public:
    Filter(const std::string& filterName, FilterCategory cat);
    virtual ~Filter();

    virtual void apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) = 0;

    const std::string& getName() const { return name; }
    FilterCategory getCategory() const { return category; }
};
