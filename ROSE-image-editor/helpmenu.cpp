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

#include "helpmenu.h"

bool HelpMenu::AboutROSE = false;
bool HelpMenu::KeyboardShortCut = false;

void HelpMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("About ROSE Editor",NULL,&AboutROSE)) { /* About */ }
        if (ImGui::MenuItem("Keyboard Shortcuts", "Ctrl+Alt+K",&KeyboardShortCut)) { /* Shortcuts */ }
        ImGui::EndMenu();
    }
}

void HelpMenu::DrawWindow()
{

}