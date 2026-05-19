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
        if (ImGui::MenuItem("Rulers", "Ctrl+R",&Rulers)) { /* Rulers */ }
        if (ImGui::MenuItem("Grid", "Ctrl+'",&Grid)) { /* Grid */ }
        if (ImGui::MenuItem("Snap", "Ctrl+Shift+;",&Snap)) { /* Snap */ }
        ImGui::EndMenu();
    }
}

void ViewMenu::DrawWindow()
{

}