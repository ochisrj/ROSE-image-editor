#include "ImageLayer.h"
#include "rendering/Framebuffer.h"

namespace layers {

ImageLayer::ImageLayer(const std::string& name, int id)
    : Layer(name, id), pixelBuffer(nullptr) {
}

void ImageLayer::render(rendering::Framebuffer* framebuffer) {
    if (!pixelBuffer || !framebuffer) {
        return;
    }
    framebuffer->drawPixelBuffer(pixelBuffer, getOpacity(), getBlendMode());
}

std::unique_ptr<Layer> ImageLayer::clone() const {
    auto cloned = std::make_unique<ImageLayer>(getName(), getId());
    cloned->setVisible(isVisible());
    cloned->setOpacity(getOpacity());
    cloned->setBlendMode(getBlendMode());
    cloned->pixelBuffer = pixelBuffer;
    return cloned;
}

void ImageLayer::setImage(image::PixelBuffer* buffer) {
    pixelBuffer = buffer;
}

image::PixelBuffer* ImageLayer::getImage() const {
    return pixelBuffer;
}

} // namespace layers
