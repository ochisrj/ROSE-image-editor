#include "tools/EraserTool.h"

EraserTool::EraserTool()
    : Tool("Eraser", CursorIcon::Eraser), size(10.0f), hardness(0.8f) {
}

EraserTool::~EraserTool() {
}

void EraserTool::onMouseDown(float x, float y, int button) {
    // Stub: Start erasing
}

void EraserTool::onMouseMove(float x, float y) {
    // Stub: Erase pixels
}

void EraserTool::onMouseUp(float x, float y, int button) {
    // Stub: Stop erasing
}

void EraserTool::onRender() {
    // Stub: Render eraser cursor
}
