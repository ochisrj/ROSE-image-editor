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

#include "imagemenu.h"

bool ImageMenu::Adjustment = false;
bool ImageMenu::CanvasSize = false;
bool ImageMenu::ImageSize = false;
bool ImageMenu::RorateCanvas = false;
bool ImageMenu::FlipHorizontal = false;
bool ImageMenu::FlipVertical = false;

void ImageMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Image"))
    {
        if (ImGui::MenuItem("Adjustments",NULL,&Adjustment)) { /* Adjustments */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Canvas Size", "Ctrl+Alt+C",&CanvasSize)) { /* Canvas Size */ }
        if (ImGui::MenuItem("Image Size", "Ctrl+Alt+I",&ImageSize)) { /* Image Size */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Rotate Canvas",NULL,&RorateCanvas)) { /* Rotate */ }
        if (ImGui::MenuItem("Flip Horizontal",NULL,&FlipHorizontal)) { /* Flip H */ }
        if (ImGui::MenuItem("Flip Vertical",NULL,&FlipVertical)) { /* Flip V */ }
        ImGui::EndMenu();
    }
}

void ImageMenu::DrawWindow()
{

}