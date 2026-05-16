#pragma once

class Window;

class Input {
public:
    Input() = default;
    ~Input() = default;

    bool isKeyPressed(int key, const Window& window) const;
    bool isMouseButtonPressed(int button, const Window& window) const;
    void getMousePos(const Window& window, double& x, double& y) const;
};
