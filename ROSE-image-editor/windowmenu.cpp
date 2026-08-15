#include "windowmenu.h"

#include <cstdio>

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"
#include "imageviewer.h"

void WindowMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Window"))
    {
        // ---- Arrange ----
        if (ImGui::BeginMenu("Arrange"))
        {
            if (ImGui::MenuItem("Tile All Vertically"))
                App::Push(Cmd::WindowArrangeTileVertically);
            if (ImGui::MenuItem("Tile All Horizontally"))
                App::Push(Cmd::WindowArrangeTileHorizontally);
            if (ImGui::MenuItem("Consolidate All to Tabs"))
                App::Push(Cmd::WindowArrangeConsolidateTabs);
            if (ImGui::MenuItem("Float in Window"))
                App::Push(Cmd::WindowArrangeFloatInWindow);
            if (ImGui::MenuItem("Float All in Windows"))
                App::Push(Cmd::WindowArrangeFloatAllInWindows);
            ImGui::EndMenu();
        }

        // ---- Workspace ----
        if (ImGui::BeginMenu("Workspace"))
        {
            static int ws = 0;   // 0 Essentials, 1 3D, ...
            if (ImGui::MenuItem("Essentials", nullptr, ws == 0))
            { ws = 0; App::Push(Cmd::WorkspaceDefault); }
            if (ImGui::MenuItem("3D", nullptr, ws == 1))
            { ws = 1; App::Push(Cmd::Workspace3D); }
            if (ImGui::MenuItem("Graphic and Web", nullptr, ws == 2))
            { ws = 2; App::Push(Cmd::WorkspaceGraphicWeb); }
            if (ImGui::MenuItem("Motion", nullptr, ws == 3))
            { ws = 3; App::Push(Cmd::WorkspaceMotion); }
            if (ImGui::MenuItem("Painting", nullptr, ws == 4))
            { ws = 4; App::Push(Cmd::WorkspacePainting); }
            if (ImGui::MenuItem("Photography", nullptr, ws == 5))
            { ws = 5; App::Push(Cmd::WorkspacePhotography); }
            ImGui::Separator();
            if (ImGui::MenuItem("Reset Current Workspace"))
                App::Push(Cmd::WorkspaceResetCurrent);
            if (ImGui::MenuItem("New Workspace..."))
                App::Push(Cmd::WorkspaceNew);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Extensions ----
        if (ImGui::BeginMenu("Extensions"))
        {
            ImGui::TextDisabled("(No extensions installed)");
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Checkable panels ----
        ImGui::MenuItem("3D", nullptr, &App::Show3D);
        ImGui::MenuItem("Actions", "Alt+F9", &App::ShowActions);
        ImGui::MenuItem("Adjustments", nullptr, &App::ShowAdjustments);
        ImGui::MenuItem("Brush Settings", "F5", &App::ShowBrushSettings);
        ImGui::MenuItem("Brushes", nullptr, &App::ShowBrushes);
        ImGui::MenuItem("Channels", nullptr, &App::ShowChannels);
        ImGui::MenuItem("Character", nullptr, &App::ShowCharacter);
        ImGui::MenuItem("Color", "F6", &App::ShowColor);
        ImGui::MenuItem("History", nullptr, &App::ShowHistory);
        ImGui::MenuItem("Info", "F8", &App::ShowInfo);
        ImGui::MenuItem("Layers", "F7", &App::ShowLayers);
        ImGui::MenuItem("Navigator", nullptr, &App::ShowNavigator);
        ImGui::MenuItem("Paragraph", nullptr, &App::ShowParagraph);
        ImGui::MenuItem("Properties", nullptr, &App::ShowProperties);
        ImGui::MenuItem("Paths", nullptr, &App::ShowPaths);
        ImGui::MenuItem("Swatches", nullptr, &App::ShowSwatches);
        ImGui::MenuItem("Timeline", nullptr, &App::ShowTimeline);
        ImGui::MenuItem("Tool Settings", nullptr, &App::ShowToolSettings);
        ImGui::MenuItem("Viewport / Scene View", nullptr, &App::ShowViewport);

        ImGui::Separator();

        ImGui::MenuItem("Application Frame", nullptr, &App::ApplicationFrame);
        ImGui::MenuItem("Options", nullptr, &App::ShowOptions);
        ImGui::MenuItem("Tools", nullptr, &App::ShowTools);

        ImGui::EndMenu();
    }
}

void WindowMenu::DrawWindow()
{
    if (App::ShowViewport)
        ImageViewer::DrawWindow();

    if (App::ShowTools)
    {
        ImGui::SetNextWindowSize(ImVec2(220, 320), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Tools", &App::ShowTools))
        {
            static const char* const tools[] =
            {
                "Move", "Marquee", "Lasso", "Wand",
                "Crop", "Eyedropper", "Brush", "Eraser",
                "Text", "Zoom"
            };
            const float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
            for (int i = 0; i < (int)IM_ARRAYSIZE(tools); ++i)
            {
                ImGui::PushID(i);
                if (ImGui::Selectable(tools[i], false, 0, ImVec2(w, 0)))
                    App::SetStatus("Tool selected: %s", tools[i]);
                if ((i & 1) == 0 && i + 1 < (int)IM_ARRAYSIZE(tools))
                    ImGui::SameLine();
                ImGui::PopID();
            }
            ImGui::End();
        }
    }

    if (App::ShowLayers)
    {
        ImGui::SetNextWindowSize(ImVec2(260, 320), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Layers", &App::ShowLayers))
        {
            ImGui::TextDisabled("Layer stack (Phase 3)");
            for (int i = 3; i >= 0; --i)
            {
                char label[32];
                snprintf(label, sizeof(label), "Layer %d", i + 1);
                if (ImGui::Selectable(label, i == 0))
                    App::SetStatus("Selected %s", label);
            }
            ImGui::Separator();
            if (ImGui::SmallButton("New"))
                App::Push(Cmd::LayerNewLayer);
            ImGui::SameLine();
            if (ImGui::SmallButton("Delete"))
                App::Push(Cmd::LayerDeleteLayer);
            ImGui::End();
        }
    }

    if (App::ShowHistory)
    {
        ImGui::SetNextWindowSize(ImVec2(220, 200), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("History", &App::ShowHistory))
        {
            ImGui::TextDisabled("History / Undo stack (Phase 4)");
            ImGui::TextUnformatted("Initial State");
            ImGui::TextUnformatted("Open");
            ImGui::TextUnformatted("Adjustment");
            ImGui::End();
        }
    }

    if (App::ShowProperties)
    {
        ImGui::SetNextWindowSize(ImVec2(260, 220), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Properties", &App::ShowProperties))
        {
            if (App::HasDocument())
            {
                ImGui::TextUnformatted("Image");
                ImGui::Separator();
                ImGui::Text("Name: %s", ImageViewer::FileName().c_str());
                ImGui::Text("Zoom: %.1f%%", ImageViewer::GetZoom() * 100.0f);
            }
            else
            {
                ImGui::TextDisabled("Open an image to see properties.");
            }
            ImGui::End();
        }
    }

    if (App::ShowColor)
    {
        ImGui::SetNextWindowSize(ImVec2(260, 220), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Color / Swatches", &App::ShowColor))
        {
            static ImVec4 fg(1.0f, 1.0f, 1.0f, 1.0f);
            static ImVec4 bg(0.0f, 0.0f, 0.0f, 1.0f);
            ImGui::ColorEdit4("Foreground", &fg.x, ImGuiColorEditFlags_NoInputs);
            ImGui::ColorEdit4("Background", &bg.x, ImGuiColorEditFlags_NoInputs);
            ImGui::End();
        }
    }

    // ---- Generic placeholder panels (template) ----
    struct Panel { const char* name; bool* visible; };
    const Panel panels[] =
    {
        { "3D",             &App::Show3D },
        { "Actions",        &App::ShowActions },
        { "Adjustments",    &App::ShowAdjustments },
        { "Brush Settings", &App::ShowBrushSettings },
        { "Brushes",        &App::ShowBrushes },
        { "Channels",       &App::ShowChannels },
        { "Character",      &App::ShowCharacter },
        { "Info",           &App::ShowInfo },
        { "Navigator",      &App::ShowNavigator },
        { "Paragraph",      &App::ShowParagraph },
        { "Paths",          &App::ShowPaths },
        { "Swatches",       &App::ShowSwatches },
        { "Timeline",       &App::ShowTimeline },
        { "Tool Settings",  &App::ShowToolSettings },
    };
    for (const Panel& p : panels)
    {
        if (!*p.visible)
            continue;
        ImGui::SetNextWindowSize(ImVec2(240, 180), ImGuiCond_FirstUseEver);
        if (ImGui::Begin(p.name, p.visible))
        {
            ImGui::TextDisabled("%s panel (template stub)", p.name);
        }
        ImGui::End();
    }
}