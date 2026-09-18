#include "menubar.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"
#include "statusbar.h"
#include "PreferencesWindow.h"
#include "LanguageTest.h"
#include <GLFW/glfw3.h>

void MenuBar::Draw(GLFWwindow* window)
{
    RenderMainMenuBar(window);
    StatusBar::ReserveBottomSpace();
    RenderMenuWindows();
    App::HandleGlobalShortcuts();
    App::Dispatch();
    StatusBar::RenderStatusBar();
}

void MenuBar::RenderMainMenuBar(GLFWwindow* window)
{
    if (ImGui::BeginMainMenuBar())
    {
        FileMenu::DrawMenu(window);
        EditMenu::DrawMenu();
        ImageMenu::DrawMenu();
        LayerMenu::DrawMenu();
        SelectMenu::DrawMenu();
        FilterMenu::DrawMenu();
        ViewMenu::DrawMenu();
        WindowMenu::DrawMenu();
        HelpMenu::DrawMenu();

        ImGui::EndMainMenuBar();
    }
}

void MenuBar::RenderMenuWindows()
{
    FileMenu::DrawWindow();
    EditMenu::DrawWindow(); // now a no-op stub (logic moved to PreferencesWindow)
    ImageMenu::DrawWindow();
    LayerMenu::DrawWindow();
    SelectMenu::DrawWindow();
    FilterMenu::DrawWindow();
    ViewMenu::DrawWindow();
    WindowMenu::DrawWindow();
    HelpMenu::DrawWindow();

    // Dedicated Photoshop-style Preferences modal (owns "Preferences" popup ID)
    PreferencesWindow::Render();

    // Thai Language Test window
    LanguageTest::Draw();
}