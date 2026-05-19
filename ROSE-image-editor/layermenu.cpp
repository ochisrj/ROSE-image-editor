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

#include "layermenu.h"

bool LayerMenu::NewLayer = false;
bool LayerMenu::DuplicateLayer = false;
bool LayerMenu::DeleteLayer = false;
bool LayerMenu::LayerStyle = false;
bool LayerMenu::MergeDown = false;
bool LayerMenu::FlattenImage = false;

void LayerMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Layer"))
    {
        if (ImGui::MenuItem("New Layer", "Ctrl+Shift+N",&NewLayer)) { /* New Layer */ }
        if (ImGui::MenuItem("Duplicate Layer","Ctrl+Shift+D",&DuplicateLayer)) { /* Duplicate */ }
        if (ImGui::MenuItem("Delete Layer", "Del",&DeleteLayer)) { /* Delete */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Layer Style",NULL,&LayerStyle)) { /* Layer Style */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Merge Down", "Ctrl+E",&MergeDown)) { /* Merge */ }
        if (ImGui::MenuItem("Flatten Image",NULL,&FlattenImage)) { /* Flatten */ }
        ImGui::EndMenu();
    }
}

void LayerMenu::DrawWindow()
{

}