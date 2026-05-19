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

#include "filtermenu.h"

bool FilterMenu::Blurs = false;
bool FilterMenu::GaussianBlur = false;
bool FilterMenu::MotionBlur = false;
bool FilterMenu::BoxBlur = false;
bool FilterMenu::Sharpen = false;
bool FilterMenu::UnsharpedMask = false;
bool FilterMenu::SmartSharpen = false;
bool FilterMenu::Distort = false;
bool FilterMenu::Liquify;
bool FilterMenu::Wave = false;
bool FilterMenu::Noise = false;
bool FilterMenu::AddNoise = false;
bool FilterMenu::Despeckle = false;
bool FilterMenu::LastFilter = false;

void FilterMenu::DrawMenu()
{

    if (ImGui::BeginMenu("Filter"))
    {
        if (ImGui::BeginMenu("Blur",&Blurs))
        {
            if (ImGui::MenuItem("Gaussian Blur",NULL,&GaussianBlur)) { /* Gaussian */ }
            if (ImGui::MenuItem("Motion Blur",NULL,&MotionBlur)) { /* Motion */ }
            if (ImGui::MenuItem("Box Blur",NULL,&BoxBlur)) { /* Box */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Sharpen",&Sharpen))
        {
            if (ImGui::MenuItem("Unsharp Mask",NULL,&UnsharpedMask)) { /* Unsharp */ }
            if (ImGui::MenuItem("Smart Sharpen",NULL,&SmartSharpen)) { /* Smart */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Distort",&Distort))
        {
            if (ImGui::MenuItem("Liquify",NULL,&Liquify)) { /* Liquify */ }
            if (ImGui::MenuItem("Wave",NULL,&Wave)) { /* Wave */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Noise",&Noise))
        {
            if (ImGui::MenuItem("Add Noise",NULL,&AddNoise)) { /* Add */ }
            if (ImGui::MenuItem("Despeckle",NULL,&Despeckle)) { /* Despeckle */ }
            ImGui::EndMenu();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Last Filter",NULL,&LastFilter)) { /* Last Filter */ }
        ImGui::EndMenu();
    }
}

void FilterMenu::DrawWindow()
{

}