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

#include "filemenu.h"

bool FileMenu::NewFile = false;
bool FileMenu::OpenFile = false;
bool FileMenu::SaveFile = false;
bool FileMenu::SaveAsFile = false;
bool FileMenu::ExportFile = false;
bool FileMenu::Exit = false;

void FileMenu::DrawMenu()
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New", "Ctrl+N",&NewFile)) { /* New file */ }
        if (ImGui::MenuItem("Open", "Ctrl+O",&OpenFile)) { /* Open file */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Save", "Ctrl+S",&SaveFile)) { /* Save file */ }
        if (ImGui::MenuItem("Save As", "Ctrl+Shift+S",&SaveAsFile)) { /* Save As */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Export As",NULL,&ExportFile)) { /* Export */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Alt+F4",&Exit))
        {
            /*
            GLFWwindow* currentWindow = glfwGetCurrentContext();
            if (currentWindow != nullptr)
            {
                glfwSetWindowShouldClose(currentWindow, true);
            }
            */
        }
        ImGui::EndMenu();
    }
}

void FileMenu::DrawWindow()
{

}