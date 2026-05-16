#include "tools/GradientTool.h"

GradientTool::GradientTool()
    : Tool("Gradient", CursorIcon::Crosshair), gradientType(GradientType::Linear),
      startColor(0xFF000000), endColor(0xFFFFFFFF),
      startPoint(0.0f, 0.0f), endPoint(0.0f, 0.0f) {
}

GradientTool::~GradientTool() {
}

void GradientTool::onMouseDown(float x, float y, int button) {
    // Stub: Start gradient
}

void GradientTool::onMouseMove(float x, float y) {
    // Stub: Update gradient end point
}

void GradientTool::onMouseUp(float x, float y, int button) {
    // Stub: Apply gradient
}

void GradientTool::onRender() {
    // Stub: Render gradient preview
}
