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

bool EditMenu::Undo = false;
bool EditMenu::Redo = false;
bool EditMenu::Cut = false;
bool EditMenu::Copy = false;
bool EditMenu::Paste = false;
bool EditMenu::Performace = false;
bool EditMenu::TriangleConfig = false;

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
        if (ImGui::MenuItem("Triangle Setting","Ctrl+M",&TriangleConfig)){}
        ImGui::EndMenu();
    }
}

void EditMenu::DrawWindow()
{
    if (TriangleConfig)
    {
        ImGui::Begin("Triangle Position/Color",&TriangleConfig);
        static float rotation = 0.0;
        ImGui::SliderFloat("rotation", &rotation, 0, 2);
        static float translation[] = { 0.0, 0.0 };
        ImGui::SliderFloat2("position", translation, -1.0, 1.0);
        static float color[4] = { 1.0f,1.0f,1.0f,1.0f };
        ImGui::End();
    }

}