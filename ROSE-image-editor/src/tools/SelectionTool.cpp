#include "tools/SelectionTool.h"

SelectionTool::SelectionTool()
    : Tool("Selection", CursorIcon::Crosshair), selectionType(SelectionType::Rect), startPoint(0.0f, 0.0f), endPoint(0.0f, 0.0f) {
}

SelectionTool::~SelectionTool() {
}

void SelectionTool::onMouseDown(float x, float y, int button) {
    // Stub: Start selection
}

void SelectionTool::onMouseMove(float x, float y) {
    // Stub: Update selection region
}

void SelectionTool::onMouseUp(float x, float y, int button) {
    // Stub: Finalize selection
}

void SelectionTool::onRender() {
    // Stub: Render selection outline
}
