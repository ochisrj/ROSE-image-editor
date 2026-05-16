#pragma once

#include "core/Window.h"
#include "core/Input.h"
#include "core/Config.h"

class ImGuiLayer;
class MenuBar;
class DocumentManager;

class Application {
public:
    Application();
    ~Application();

    void Run();
    void Shutdown();

private:
    void init();
    void update();
    void render();

    Window m_Window;
    Input m_Input;
    Config m_Config;
    ImGuiLayer* m_ImGuiLayer;
    MenuBar* m_MenuBar;
    DocumentManager* m_DocumentManager;
    bool m_Running;
};
