#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "implot3d.h"
#include "implot_internal.h"
#include "implot3d_internal.h"

#include "viewmenu.h"

bool ViewMenu::ZoomIn = false;
bool ViewMenu::ZoomOut = false;
bool ViewMenu::FitOnScreen = false;
bool ViewMenu::Toolbar = false;
bool ViewMenu::ToolOption = false;
bool ViewMenu::Panels = false;
bool ViewMenu::TabMenu = false;
bool ViewMenu::Workspace = false;
bool ViewMenu::Rulers = false;
bool ViewMenu::Grid = false;
bool ViewMenu::Snap = false;

void ViewMenu::DrawMenu()
{
    if (ImGui::BeginMenu("View"))
    {
        if (ImGui::MenuItem("Zoom In", "Ctrl+=",&ZoomIn)) { /* Zoom In */ }
        if (ImGui::MenuItem("Zoom Out", "Ctrl+-",&ZoomOut)) { /* Zoom Out */ }
        if (ImGui::MenuItem("Fit on Screen", "Ctrl+0",&FitOnScreen)) { /* Fit */ }
        ImGui::Separator();
        if(ImGui::MenuItem("Toolbar",NULL,&Toolbar)){}
        if(ImGui::MenuItem("Tool Opetion",NULL,&ToolOption)){}
        if(ImGui::MenuItem("Panels",NULL,&Panels)){}
        if(ImGui::MenuItem("Tab Menu",NULL,&TabMenu)){}
        if(ImGui::MenuItem("Workspace",NULL,&Workspace)){}
        ImGui::Separator();
        if (ImGui::MenuItem("Rulers", "Ctrl+R",&Rulers)) { /* Rulers */ }
        if (ImGui::MenuItem("Grid", "Ctrl+'",&Grid)) { /* Grid */ }
        if (ImGui::MenuItem("Snap", "Ctrl+Shift+;",&Snap)) { /* Snap */ }
        ImGui::EndMenu();
    }
}

void ViewMenu::DrawWindow()
{
    if (Toolbar)
    {
        ImGuiWindowFlags WindowFlag = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar;
        if (ImGui::Begin("workspace",&Toolbar,WindowFlag))
        {
            float itemWidth = 50.f;
            float itemHeight = 50.f;
            float spacing = 8.f;

            // Fetch width available in the current window viewport
            float windowVisibleX2 = ImGui::GetWindowPos().x + ImGui::GetContentRegionAvail().x;

            for (int i = 0; i < 10; i++)
            {
                ImGui::PushID(i);

                // Draw your grid item (e.g., custom image button or sub-menu box)
                ImGui::Button("Cell", ImVec2(itemWidth, itemHeight));

                // Calculate position bounds for wrapping
                float lastItemX2 = ImGui::GetItemRectMax().x;
                float nextItemX2 = lastItemX2 + spacing + itemWidth;

                if (i + 1 < 20 && nextItemX2 < windowVisibleX2)
                    ImGui::SameLine(0.0f, spacing); // Stay on same horizontal line

                ImGui::PopID();
            }

            ImGui::End();
        }
    }

    if (Workspace)
    {
    

    }
}