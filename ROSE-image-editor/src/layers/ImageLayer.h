#pragma once

#include "Layer.h"
#include "image/PixelBuffer.h"
#include <memory>

namespace rendering {
    class Framebuffer;
}

namespace layers {

class ImageLayer : public Layer {
private:
    image::PixelBuffer* pixelBuffer;

public:
    ImageLayer(const std::string& name, int id);
    ~ImageLayer() override = default;

    void render(rendering::Framebuffer* framebuffer) override;
    std::unique_ptr<Layer> clone() const override;

    void setImage(image::PixelBuffer* buffer);
    image::PixelBuffer* getImage() const;
};

} // namespace layers
