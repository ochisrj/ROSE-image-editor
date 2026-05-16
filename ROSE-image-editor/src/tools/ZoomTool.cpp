#include "tools/ZoomTool.h"

ZoomTool::ZoomTool()
    : Tool("Zoom", CursorIcon::Zoom), zoomFactor(1.0f) {
}

ZoomTool::~ZoomTool() {
}

void ZoomTool::onMouseDown(float x, float y, int button) {
    // Stub: Click to zoom in/out
}

void ZoomTool::onMouseMove(float x, float y) {
    // Stub: Pan or show zoom preview
}

void ZoomTool::onMouseUp(float x, float y, int button) {
    // Stub: Finalize zoom
}

void ZoomTool::onRender() {
    // Stub: Render zoom indicator
}

void ZoomTool::zoomIn() {
    zoomFactor *= 1.25f;
}

void ZoomTool::zoomOut() {
    zoomFactor *= 0.8f;
}
