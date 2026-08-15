#include "menubar.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"
#include "statusbar.h"

void MenuBar::Draw(GLFWwindow* window)
{
    RenderMainMenuBar(window);
    StatusBar::ReserveBottomSpace();
    RenderMenuWindows();
    App::HandleGlobalShortcuts();
    App::Dispatch();
    StatusBar::Draw();
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

        // Right-aligned status / FPS readout.
        if (!App::Status.empty())
        {
            ImGui::SameLine(0.0f, 0.0f);
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 260.0f);
            ImGui::TextUnformatted(App::Status.c_str());
        }
        //if (App::PrefShowFps)
        //{
        //    ImGui::SameLine(0.0f, 0.0f);
        //    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 70.0f);
        //    ImGui::TextDisabled("| %.1f FPS", ImGui::GetIO().Framerate);
        //}

        ImGui::EndMainMenuBar();
    }
}

void MenuBar::RenderMenuWindows()
{
    FileMenu::DrawWindow();
    EditMenu::DrawWindow();
    ImageMenu::DrawWindow();
    LayerMenu::DrawWindow();
    SelectMenu::DrawWindow();
    FilterMenu::DrawWindow();
    ViewMenu::DrawWindow();
    WindowMenu::DrawWindow();
    HelpMenu::DrawWindow();
}