#include "helpmenu.h"

#include <glad/glad.h>

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"

void HelpMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("ROSE Image Editor Help...", "F1"))
            App::Push(Cmd::HelpDocumentation);
        if (ImGui::MenuItem("ROSE Tutorials..."))
            App::Push(Cmd::HelpTutorials);
        if (ImGui::MenuItem("Hands-on Tutorials"))
            App::Push(Cmd::HelpHandsOnTutorials);

        ImGui::Separator();

        if (ImGui::MenuItem("About ROSE Image Editor..."))
            App::Push(Cmd::HelpAbout);
        if (ImGui::MenuItem("System Info..."))
            App::Push(Cmd::HelpSystemInfo);
        if (ImGui::MenuItem("Manage My Account..."))
            App::Push(Cmd::HelpManageAccount);

        ImGui::Separator();

        if (ImGui::MenuItem("Sign Out..."))
            App::Push(Cmd::HelpSignOut);

        ImGui::EndMenu();
    }
}

void HelpMenu::DrawWindow()
{
    if (App::ShowAbout)
    {
        ImGui::OpenPopup("About ROSE Image Editor");
        App::ShowAbout = false;
    }

    bool open = true;
    if (ImGui::BeginPopupModal("About ROSE Image Editor", &open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::TextUnformatted("ROSE Image Editor");
        ImGui::Separator();
        ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
        const GLubyte* renderer = glGetString(GL_RENDERER);
        const GLubyte* version  = glGetString(GL_VERSION);
        ImGui::Text("Renderer: %s", renderer ? (const char*)renderer : "N/A");
        ImGui::Text("OpenGL:   %s", version ? (const char*)version : "N/A");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0)))
            open = false;
        ImGui::EndPopup();
    }

    if (App::ShowKeyboardShortcuts)
    {
        ImGui::OpenPopup("Keyboard Shortcuts");
        App::ShowKeyboardShortcuts = false;
    }

    if (ImGui::BeginPopupModal("Keyboard Shortcuts", &open, ImGuiWindowFlags_NoResize))
    {
        ImGui::BeginChild("##shortcuts_scroll", ImVec2(520, 380), ImGuiChildFlags_Borders);
        int count = 0;
        const App::ShortcutEntry* sc = App::GetShortcuts(&count);
        if (ImGui::BeginTable("##shortcuts_table", 2,
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV))
        {
            ImGui::TableSetupColumn("Command", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Shortcut", ImGuiTableColumnFlags_WidthFixed, 160.0f);
            ImGui::TableHeadersRow();
            for (int i = 0; i < count; ++i)
            {
                if (!sc[i].showInHelp)
                    continue;
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(sc[i].label);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(sc[i].shortcut);
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();

        if (ImGui::Button("Close", ImVec2(120, 0)))
            open = false;
        ImGui::EndPopup();
    }
}