#include "filters/SharpenFilter.h"
#include <vector>
#include <cmath>
#include <algorithm>

SharpenFilter::SharpenFilter()
    : Filter("Sharpen", FilterCategory::Sharpen), amount(0.5f) {
}

SharpenFilter::~SharpenFilter() {
}

void SharpenFilter::apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) {
    float a = amount;
    auto it = params.find("amount");
    if (it != params.end()) {
        a = it->second;
    }

    int width = pixelBuffer.getWidth();
    int height = pixelBuffer.getHeight();

    // Create blurred copy for unsharp mask
    std::vector<uint32_t> blurredBuffer(width * height, 0);
    int blurRadius = 1;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sumR = 0, sumG = 0, sumB = 0, sumA = 0;
            int count = 0;
            for (int ky = -blurRadius; ky <= blurRadius; ++ky) {
                for (int kx = -blurRadius; kx <= blurRadius; ++kx) {
                    int px = std::clamp(x + kx, 0, width - 1);
                    int py = std::clamp(y + ky, 0, height - 1);
                    uint32_t pixel = pixelBuffer.getPixel(px, py);
                    sumR += (pixel >> 16) & 0xFF;
                    sumG += (pixel >> 8) & 0xFF;
                    sumB += pixel & 0xFF;
                    sumA += (pixel >> 24) & 0xFF;
                    ++count;
                }
            }
            uint32_t blurred = ((static_cast<uint32_t>(sumA / count) << 24) |
                                (static_cast<uint32_t>(sumR / count) << 16) |
                                (static_cast<uint32_t>(sumG / count) << 8) |
                                static_cast<uint32_t>(sumB / count));
            blurredBuffer[y * width + x] = blurred;
        }
    }

    // Apply unsharp mask
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint32_t original = pixelBuffer.getPixel(x, y);
            uint32_t blurred = blurredBuffer[y * width + x];

            float origR = (original >> 16) & 0xFF;
            float origG = (original >> 8) & 0xFF;
            float origB = original & 0xFF;
            float origA = (original >> 24) & 0xFF;

            float blurR = (blurred >> 16) & 0xFF;
            float blurG = (blurred >> 8) & 0xFF;
            float blurB = blurred & 0xFF;

            float sharpR = origR + a * (origR - blurR);
            float sharpG = origG + a * (origG - blurG);
            float sharpB = origB + a * (origB - blurB);

            sharpR = std::clamp(sharpR, 0.0f, 255.0f);
            sharpG = std::clamp(sharpG, 0.0f, 255.0f);
            sharpB = std::clamp(sharpB, 0.0f, 255.0f);

            uint32_t result = ((static_cast<uint32_t>(origA) << 24) |
                               (static_cast<uint32_t>(sharpR) << 16) |
                               (static_cast<uint32_t>(sharpG) << 8) |
                               static_cast<uint32_t>(sharpB));
            pixelBuffer.setPixel(x, y, result);
        }
    }
}
