#include "tools/MoveTool.h"

MoveTool::MoveTool()
    : Tool("Move", CursorIcon::Move), selectedLayerIndex(-1), dragOffset(0.0f, 0.0f) {
}

MoveTool::~MoveTool() {
}

void MoveTool::onMouseDown(float x, float y, int button) {
    // Stub: Start dragging layer
}

void MoveTool::onMouseMove(float x, float y) {
    // Stub: Move selected layer
}

void MoveTool::onMouseUp(float x, float y, int button) {
    // Stub: Stop dragging
}

void MoveTool::onRender() {
    // Stub: Render move handles
}
