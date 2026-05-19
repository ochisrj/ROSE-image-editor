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

#include "editmenu.h"

bool EditMenu::Undo;
bool EditMenu::Redo;
bool EditMenu::Cut;
bool EditMenu::Copy;
bool EditMenu::Paste;
bool EditMenu::Performace;

void EditMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "Ctrl+Z",&Undo)) { /* Undo */ }
        if (ImGui::MenuItem("Redo", "Ctrl+Y",&Redo)) { /* Redo */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Cut", "Ctrl+X",&Cut)) { /* Cut */ }
        if (ImGui::MenuItem("Copy", "Ctrl+C",&Copy)) { /* Copy */ }
        if (ImGui::MenuItem("Paste", "Ctrl+V",&Paste)) { /* Paste */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Preferences", "Ctrl+K",&Performace)) { /* Preferences */ }
        ImGui::EndMenu();
    }
}

void EditMenu::DrawWindow()
{

}