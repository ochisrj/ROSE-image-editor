#include "tools/CropTool.h"

CropTool::CropTool()
    : Tool("Crop", CursorIcon::Crop), cropRect(0.0f, 0.0f, 0.0f, 0.0f) {
}

CropTool::~CropTool() {
}

void CropTool::onMouseDown(float x, float y, int button) {
    // Stub: Start crop region
}

void CropTool::onMouseMove(float x, float y) {
    // Stub: Update crop region
}

void CropTool::onMouseUp(float x, float y, int button) {
    // Stub: Finalize crop region
}

void CropTool::onRender() {
    // Stub: Render crop overlay
}

void CropTool::applyCrop() {
    // Stub: Apply crop to canvas
}
