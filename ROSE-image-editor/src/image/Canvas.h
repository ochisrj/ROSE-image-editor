#pragma once

namespace image {

class Canvas {
private:
    float zoom;
    float panX;
    float panY;

public:
    Canvas();
    ~Canvas();

    void setZoom(float z);
    float getZoom() const { return zoom; }

    void pan(float dx, float dy);

    void screenToCanvas(float sx, float sy, float& cx, float& cy) const;
    void canvasToScreen(float cx, float cy, float& sx, float& sy) const;

    void resetView();
};

}
