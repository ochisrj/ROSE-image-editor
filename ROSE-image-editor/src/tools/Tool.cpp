#include "tools/Tool.h"

Tool::Tool(const std::string& toolName, CursorIcon icon)
    : name(toolName), cursorIcon(icon), isActive(false) {
}

Tool::~Tool() {
}
