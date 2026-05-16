#include "filters/BlurFilter.h"
#include <vector>
#include <cmath>
#include <algorithm>

BlurFilter::BlurFilter()
    : Filter("Gaussian Blur", FilterCategory::Blur), radius(1.0f) {
}

BlurFilter::~BlurFilter() {
}

void BlurFilter::apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) {
    float r = radius;
    auto it = params.find("radius");
    if (it != params.end()) {
        r = it->second;
    }

    int width = pixelBuffer.getWidth();
    int height = pixelBuffer.getHeight();
    int radiusInt = static_cast<int>(std::ceil(r));
    int kernelSize = radiusInt * 2 + 1;

    std::vector<uint32_t> tempBuffer(width * height, 0);

    // Horizontal blur pass
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sumR = 0, sumG = 0, sumB = 0, sumA = 0;
            int count = 0;
            for (int kx = -radiusInt; kx <= radiusInt; ++kx) {
                int px = std::clamp(x + kx, 0, width - 1);
                uint32_t pixel = pixelBuffer.getPixel(px, y);
                sumR += (pixel >> 16) & 0xFF;
                sumG += (pixel >> 8) & 0xFF;
                sumB += pixel & 0xFF;
                sumA += (pixel >> 24) & 0xFF;
                ++count;
            }
            uint32_t blurred = ((static_cast<uint32_t>(sumA / count) << 24) |
                                (static_cast<uint32_t>(sumR / count) << 16) |
                                (static_cast<uint32_t>(sumG / count) << 8) |
                                static_cast<uint32_t>(sumB / count));
            tempBuffer[y * width + x] = blurred;
        }
    }

    // Vertical blur pass
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sumR = 0, sumG = 0, sumB = 0, sumA = 0;
            int count = 0;
            for (int ky = -radiusInt; ky <= radiusInt; ++ky) {
                int py = std::clamp(y + ky, 0, height - 1);
                uint32_t pixel = tempBuffer[py * width + x];
                sumR += (pixel >> 16) & 0xFF;
                sumG += (pixel >> 8) & 0xFF;
                sumB += pixel & 0xFF;
                sumA += (pixel >> 24) & 0xFF;
                ++count;
            }
            uint32_t blurred = ((static_cast<uint32_t>(sumA / count) << 24) |
                                (static_cast<uint32_t>(sumR / count) << 16) |
                                (static_cast<uint32_t>(sumG / count) << 8) |
                                static_cast<uint32_t>(sumB / count));
            pixelBuffer.setPixel(x, y, blurred);
        }
    }
}
