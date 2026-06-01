#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "filemenu.h"

bool FileMenu::NewFile = false;
bool FileMenu::OpenFile = false;
bool FileMenu::SaveFile = false;
bool FileMenu::SaveAsFile = false;
bool FileMenu::ExportFile = false;
bool FileMenu::Preference = false;
bool FileMenu::Exit = false;

void FileMenu::DrawMenu(GLFWwindow* window)
{
    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("New", "Ctrl+N")) { NewFile = true; }
        if (ImGui::MenuItem("Open", "Ctrl+O")) { OpenFile = true; }
        ImGui::Separator();
        if (ImGui::MenuItem("Save", "Ctrl+S")) { SaveFile = true; }
        if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) { SaveAsFile = true; }
        ImGui::Separator();
        if (ImGui::MenuItem("Export As")) { ExportFile = true; }
        if (ImGui::MenuItem("Preferences")) { Preference = true; }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "Alt+F4"))
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        ImGui::EndMenu();
    }
}

void FileMenu::DrawWindow()
{

}