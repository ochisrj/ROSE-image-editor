#include "Canvas.h"
#include <algorithm>

namespace image {

Canvas::Canvas() : zoom(1.0f), panX(0.0f), panY(0.0f) {}

Canvas::~Canvas() {}

void Canvas::setZoom(float z) {
    zoom = std::max(0.1f, std::min(z, 100.0f));
}

void Canvas::pan(float dx, float dy) {
    panX += dx;
    panY += dy;
}

void Canvas::screenToCanvas(float sx, float sy, float& cx, float& cy) const {
    cx = (sx - panX) / zoom;
    cy = (sy - panY) / zoom;
}

void Canvas::canvasToScreen(float cx, float cy, float& sx, float& sy) const {
    sx = cx * zoom + panX;
    sy = cy * zoom + panY;
}

void Canvas::resetView() {
    zoom = 1.0f;
    panX = 0.0f;
    panY = 0.0f;
}

}
