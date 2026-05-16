#pragma once

#include <string>
#include <cstdint>

enum class CursorIcon {
    Default,
    Crosshair,
    Hand,
    Move,
    Text,
    Brush,
    Eraser,
    Crop,
    Zoom,
    Grab,
    Eyedropper
};

class Tool {
protected:
    std::string name;
    CursorIcon cursorIcon;
    bool isActive;

public:
    Tool(const std::string& toolName, CursorIcon icon);
    virtual ~Tool();

    virtual void onMouseDown(float x, float y, int button) = 0;
    virtual void onMouseMove(float x, float y) = 0;
    virtual void onMouseUp(float x, float y, int button) = 0;
    virtual void onRender() = 0;

    const std::string& getName() const { return name; }
    CursorIcon getCursorIcon() const { return cursorIcon; }
    bool getIsActive() const { return isActive; }
    void setIsActive(bool active) { isActive = active; }
};
