#include "tools/BrushTool.h"

BrushTool::BrushTool()
    : Tool("Brush", CursorIcon::Brush), size(10.0f), hardness(0.8f), opacity(1.0f), color(0xFF000000) {
}

BrushTool::~BrushTool() {
}

void BrushTool::onMouseDown(float x, float y, int button) {
    // Stub: Start brush stroke
}

void BrushTool::onMouseMove(float x, float y) {
    // Stub: Draw brush stroke to canvas
}

void BrushTool::onMouseUp(float x, float y, int button) {
    // Stub: End brush stroke
}

void BrushTool::onRender() {
    // Stub: Render brush cursor
}
