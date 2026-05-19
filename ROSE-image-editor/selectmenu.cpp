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

#include "selectmenu.h"

bool SelectMenu::SelectAll = false;
bool SelectMenu::Deselect = false;
bool SelectMenu::Inverse = false;
bool SelectMenu::ColorRange = false;
bool SelectMenu::Modify = false;

void SelectMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Select"))
    {
        if (ImGui::MenuItem("All", "Ctrl+A",&SelectAll)) { /* Select All */ }
        if (ImGui::MenuItem("Deselect", "Ctrl+D",&Deselect)) { /* Deselect */ }
        if (ImGui::MenuItem("Inverse", "Ctrl+Shift+I",&Inverse)) { /* Inverse */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Color Range",NULL,&ColorRange)) { /* Color Range */ }
        if (ImGui::MenuItem("Modify",NULL,&Modify)) { /* Modify */ }
        ImGui::EndMenu();
    }
}

void SelectMenu::DrawWindow()
{

}