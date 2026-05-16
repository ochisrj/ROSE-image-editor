#include "tools/TextTool.h"

TextTool::TextTool()
    : Tool("Text", CursorIcon::Text), textBuffer(""), fontSize(12.0f), color(0xFF000000), position(0.0f, 0.0f) {
}

TextTool::~TextTool() {
}

void TextTool::onMouseDown(float x, float y, int button) {
    // Stub: Place text cursor
}

void TextTool::onMouseMove(float x, float y) {
    // Stub: Move text position
}

void TextTool::onMouseUp(float x, float y, int button) {
    // Stub: Finalize text placement
}

void TextTool::onRender() {
    // Stub: Render text preview
}
