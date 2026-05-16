#include "filters/DistortFilter.h"
#include <vector>
#include <cmath>
#include <algorithm>

DistortFilter::DistortFilter()
    : Filter("Distort", FilterCategory::Distort), distortionType(DistortionType::Wave), amount(0.5f) {
}

DistortFilter::~DistortFilter() {
}

void DistortFilter::apply(PixelBuffer& pixelBuffer, const std::unordered_map<std::string, float>& params) {
    float a = amount;
    auto it = params.find("amount");
    if (it != params.end()) {
        a = it->second;
    }

    int width = pixelBuffer.getWidth();
    int height = pixelBuffer.getHeight();
    std::vector<uint32_t> tempBuffer(width * height, 0);

    // Copy original to temp buffer
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            tempBuffer[y * width + x] = pixelBuffer.getPixel(x, y);
        }
    }

    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    float maxDistance = std::sqrt(centerX * centerX + centerY * centerY);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float srcX = static_cast<float>(x);
            float srcY = static_cast<float>(y);
            float dx = srcX - centerX;
            float dy = srcY - centerY;
            float distance = std::sqrt(dx * dx + dy * dy);

            switch (distortionType) {
                case DistortionType::Wave: {
                    float wave = std::sin(distance * 0.1f) * a * 10.0f;
                    srcX += wave;
                    srcY += wave;
                    break;
                }
                case DistortionType::Spherize: {
                    float factor = 1.0f + a * (1.0f - distance / maxDistance);
                    srcX = centerX + dx * factor;
                    srcY = centerY + dy * factor;
                    break;
                }
                case DistortionType::Pinch: {
                    float factor = 1.0f - a * (1.0f - distance / maxDistance);
                    srcX = centerX + dx * factor;
                    srcY = centerY + dy * factor;
                    break;
                }
            }

            int sampleX = static_cast<int>(std::clamp(srcX, 0.0f, width - 1.0f));
            int sampleY = static_cast<int>(std::clamp(srcY, 0.0f, height - 1.0f));
            uint32_t sample = tempBuffer[sampleY * width + sampleX];
            pixelBuffer.setPixel(x, y, sample);
        }
    }
}
