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

#include "windowmenu.h"

bool WindowMenu::LayerPanel = false;
bool WindowMenu::Channels = false;
bool WindowMenu::History = false;
bool WindowMenu::Toolbar = false;
bool WindowMenu::Performace = false;
bool WindowMenu::ResetWorkspace = false;
bool WindowMenu::PlotTwoD = false;

void WindowMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Window"))
    {
        if (ImGui::MenuItem("Layers Panel", "F7",&LayerPanel)) { /* Layers */ }
        if (ImGui::MenuItem("Channels",NULL,&Channels)) { /* Channels */ }
        if (ImGui::MenuItem("History",NULL,&History)) { /* History */ }
        if (ImGui::MenuItem("Toolbar",NULL,&Toolbar)) { /* Toolbar */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Performance", NULL,&Performace)) { /* Performance */ }
        ImGui::Separator();
        if (ImGui::MenuItem("Reset Workspace",NULL,&ResetWorkspace)) { /* Reset */ }
        if (ImGui::MenuItem("Plot2D",NULL,&PlotTwoD)){}
        ImGui::EndMenu();
    }
}

void WindowMenu::DrawWindow()
{
    if (!Performace)
    {
        // Ring-buffer for the last 120 FPS samples
        static float  fps_history[120] = {};
        static int    fps_offset = 0;
        static double fps_refresh_time = 0.0;

        double now = ImGui::GetTime();
        // Sample up to 60 times per second so the graph scrolls smoothly
        if (now - fps_refresh_time >= 1.0 / 60.0)
        {
            fps_history[fps_offset] = ImGui::GetIO().Framerate;
            fps_offset = (fps_offset + 1) % IM_ARRAYSIZE(fps_history);
            fps_refresh_time = now;
        }

        // Ring-buffer for frame time (ms) — same approach
        static float  ms_history[120] = {};
        static int    ms_offset = 0;
        {
            static double ms_refresh_time = 0.0;
            if (now - ms_refresh_time >= 1.0 / 60.0)
            {
                float framerate = ImGui::GetIO().Framerate;
                ms_history[ms_offset] = (framerate > 0.f) ? 1000.f / framerate : 0.f;
                ms_offset = (ms_offset + 1) % IM_ARRAYSIZE(ms_history);
                ms_refresh_time = now;
            }
        }

        ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.45f);
        ImGui::SetNextWindowSize(ImVec2(320, 0), ImGuiCond_Always);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_AlwaysAutoResize;

        ImGui::Begin("FPS window", &Performace, flags);

        float currentFPS = ImGui::GetIO().Framerate;
        float currentMS = (currentFPS > 0.f) ? 1000.f / currentFPS : 0.f;

        // ── FPS coloured text ──────────────────────────────────────────────────
        ImVec4 fpsColor = (currentFPS >= 55.f) ? ImVec4(0, 1, 0, 1)
            : (currentFPS >= 30.f) ? ImVec4(1, 1, 0, 1)
            : ImVec4(1, 0, 0, 1);
        ImGui::TextColored(fpsColor, "FPS: %.1f", currentFPS);
        ImGui::SameLine();
        ImGui::TextDisabled("(%.2f ms)", currentMS);

        // ── FPS PlotLines — the graph follows the real framerate ───────────────
        ImGui::SeparatorText("FPS History");
        {
            float max_fps = 0.0f;
            float min_fps = FLT_MAX;
            for (int i = 0; i < 120; i++) {
                if (fps_history[i] > max_fps) max_fps = fps_history[i];
                if (fps_history[i] < min_fps) min_fps = fps_history[i];
            }

            char overlay[32];
            snprintf(overlay, sizeof(overlay), "%.0f fps", currentFPS);
            ImGui::PlotLines("##fps",
                fps_history, IM_ARRAYSIZE(fps_history), fps_offset,
                overlay,
                FLT_MAX, FLT_MAX,      // เปลี่ยนจาก 0.0f, 200.0f เป็น FLT_MAX ทั้งคู่
                ImVec2(-1, 70));
        }

        // ── Frame-time PlotLines ───────────────────────────────────────────────
        ImGui::SeparatorText("Frame Time (ms)");
        {
            char overlay[32];
            snprintf(overlay, sizeof(overlay), "%.2f ms", currentMS);
            ImGui::PlotLines("##ms",
                ms_history, IM_ARRAYSIZE(ms_history), ms_offset,
                overlay,
                0.0f, 50.0f,           // y-axis: 0 – 50 ms
                ImVec2(-1, 70));
        }

        // ── Target budget indicator ────────────────────────────────────────────
        ImGui::Spacing();
        ImGui::ProgressBar(currentFPS / 60.f, ImVec2(-1, 8), "");
        ImGui::TextDisabled("Budget vs 60 fps target");

        ImGui::End();


    }

    /*
    // 2. PLOT 2D WINDOW (GeoGebra Style)
    if (!PlotTwoD)
    {
        static char formula_buffer[128] = "sin(x)";
        ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("2D Function Visualizer", &PlotTwoD))
        {
            ImGui::Text("Enter Function:");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputText("##formula", formula_buffer, sizeof(formula_buffer));
            ImGui::TextDisabled("Try: x, x*x, sin(x), cos(x), tan(x)");

            ImGui::Separator();

            if (ImPlot::BeginPlot("##Graph", ImVec2(-1, -1)))
            {
                ImPlot::SetupAxes("x", "f(x)");
                ImPlotRect limits = ImPlot::GetPlotLimits();

                // Generate points dynamically based on view
                static double x_data[1000], y_data[1000];
                double step = (limits.X.Max - limits.X.Min) / 999.0;

                for (int i = 0; i < 1000; ++i) {
                    double x = limits.X.Min + (i * step);
                    double y = 0;

                    // Simple Parser Logic
                    std::string f = formula_buffer;
                    if (f == "x") y = x;
                    else if (f == "x*x" || f == "x^2") y = x * x;
                    else if (f == "sin(x)") y = sin(x);
                    else if (f == "cos(x)") y = cos(x);
                    else if (f == "tan(x)") y = tan(x);
                    else if (f == "sqrt(x)") y = sqrt(x);
                    else y = 0;

                    x_data[i] = x;
                    y_data[i] = y;
                }

                ImPlot::SetNextLineStyle(ImVec4(0, 1, 0.7f, 1), 2.0f);
                ImPlot::PlotLine("f(x)", x_data, y_data, 1000);
                ImPlot::EndPlot();
            }
        }
        ImGui::End();
    }
    */



}