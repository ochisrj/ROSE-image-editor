#include "viewmenu.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"

void ViewMenu::DrawMenu()
{
    if (ImGui::BeginMenu("View"))
    {
        // ---- Proof Setup ----
        if (ImGui::BeginMenu("Proof Setup"))
        {
            static int proof = 0;   // 0 = Custom, 1 = Working CMYK, ...
            if (ImGui::MenuItem("Custom...", nullptr, proof == 0))
            { proof = 0; App::Push(Cmd::ViewProofSetupCustom); }
            if (ImGui::MenuItem("Working CMYK", nullptr, proof == 1))
            { proof = 1; App::Push(Cmd::ViewProofSetupWorkingCMYK); }
            if (ImGui::MenuItem("Macintosh RGB", nullptr, proof == 2))
            { proof = 2; App::Push(Cmd::ViewProofSetupMacintoshRGB); }
            if (ImGui::MenuItem("Internet Standard RGB (sRGB)", nullptr, proof == 3))
            { proof = 3; App::Push(Cmd::ViewProofSetupInternetStandardRGB); }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Proof Colors", "Ctrl+Y", &App::ShowProofColors))
            App::Push(Cmd::ViewProofColors);
        if (ImGui::MenuItem("Gamut Warning", "Shift+Ctrl+Y", &App::ShowGamutWarning))
            App::Push(Cmd::ViewGamutWarning);

        ImGui::Separator();

        // ---- Pixel Aspect Ratio ----
        if (ImGui::BeginMenu("Pixel Aspect Ratio"))
        {
            static int par = 0;
            if (ImGui::MenuItem("Square", nullptr, par == 0))
            { par = 0; App::Push(Cmd::ViewPixelAspectRatioSquare); }
            if (ImGui::MenuItem("D1 VTS Aspect", nullptr, par == 1))
            { par = 1; App::Push(Cmd::ViewPixelAspectRatioD1VTSAspect); }
            if (ImGui::MenuItem("D1 PAL", nullptr, par == 2))
            { par = 2; App::Push(Cmd::ViewPixelAspectRatioD1PAL); }
            if (ImGui::MenuItem("HDV 1080", nullptr, par == 3))
            { par = 3; App::Push(Cmd::ViewPixelAspectRatioHDV1080); }
            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Pixel Aspect Ratio Reset"))
            App::Push(Cmd::ViewPixelAspectRatioReset);

        ImGui::Separator();

        // ---- Zoom ----
        if (ImGui::MenuItem("Zoom In", "Ctrl++"))
            App::Push(Cmd::ViewZoomIn);
        if (ImGui::MenuItem("Zoom Out", "Ctrl+-"))
            App::Push(Cmd::ViewZoomOut);
        if (ImGui::MenuItem("Fit on Screen", "Ctrl+0"))
            App::Push(Cmd::ViewFitScreen);
        if (ImGui::MenuItem("100% / Actual Pixels", "Ctrl+1"))
            App::Push(Cmd::ViewActualPixels);
        if (ImGui::MenuItem("Print Size"))
            App::Push(Cmd::ViewPrintSize);

        ImGui::Separator();

        // ---- Screen Mode ----
        if (ImGui::BeginMenu("Screen Mode"))
        {
            static int screen = 0;
            if (ImGui::MenuItem("Standard Screen Mode", nullptr, screen == 0))
            { screen = 0; App::ScreenMode = 0; App::Push(Cmd::ViewScreenModeStandard); }
            if (ImGui::MenuItem("Full Screen Mode With Menu Bar", "F", screen == 1))
            { screen = 1; App::ScreenMode = 1; App::Push(Cmd::ViewScreenModeFullWithMenu); }
            if (ImGui::MenuItem("Full Screen Mode", "F", screen == 2))
            { screen = 2; App::ScreenMode = 2; App::Push(Cmd::ViewScreenModeFull); }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Workspace display toggles ----
        if (ImGui::BeginMenu("Show"))
        {
            ImGui::MenuItem("Target Path", nullptr, &App::ShowTargetPath);
            ImGui::MenuItem("Canvas Guides", nullptr, &App::ShowCanvasGuides);
            ImGui::MenuItem("Pixel Grid", nullptr, &App::ShowPixelGrid);
            ImGui::MenuItem("Layer Edges", nullptr, &App::ShowLayerEdges);
            ImGui::MenuItem("Selection Edges", "Ctrl+H", &App::ShowSelectionEdges);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Workspace overlays (main-menu toggles mirror the global
        //      shortcuts: Ctrl+R, Ctrl+' and Ctrl+;) ----
        ImGui::MenuItem("Rulers", "Ctrl+R", &App::ShowRulers);
        ImGui::MenuItem("Grid", "Ctrl+'", &App::ShowGrid);
        ImGui::MenuItem("Guides", "Ctrl+;", &App::ShowGuides);

        // ---- Snap / Snap To ----
        if (ImGui::MenuItem("Snap", "Shift+Ctrl+;", &App::SnapEnabled))
            App::Push(Cmd::ViewSnap);

        if (ImGui::BeginMenu("Snap To"))
        {
            if (ImGui::MenuItem("Guides", nullptr, &App::SnapToGuides))
                App::Push(Cmd::ViewSnapToGuides);
            if (ImGui::MenuItem("Grid", nullptr, &App::SnapToGrid))
                App::Push(Cmd::ViewSnapToGrid);
            if (ImGui::MenuItem("Layers", nullptr, &App::SnapToLayers))
                App::Push(Cmd::ViewSnapToLayers);
            if (ImGui::MenuItem("Slices", nullptr, &App::SnapToSlices))
                App::Push(Cmd::ViewSnapToSlices);
            if (ImGui::MenuItem("Document Bounds", nullptr, &App::SnapToDocumentBounds))
                App::Push(Cmd::ViewSnapToDocumentBounds);
            ImGui::Separator();
            if (ImGui::MenuItem("All", nullptr, false, false))
                App::Push(Cmd::ViewSnapToAll);
            if (ImGui::MenuItem("None", nullptr, false, false))
                App::Push(Cmd::ViewSnapToNone);
            ImGui::EndMenu();
        }

        // ---- Guides ----
        if (ImGui::BeginMenu("Guides##submenu"))
        {
            if (ImGui::MenuItem("Add Guide..."))
                App::Push(Cmd::ViewGuideNew);
            if (ImGui::MenuItem("New Guide Layout..."))
                App::Push(Cmd::ViewGuideNewLayout);
            if (ImGui::MenuItem("Lock Guides", "Alt+Ctrl+;", &App::GuidesLocked))
                App::Push(Cmd::ViewGuideLockGuides);
            if (ImGui::MenuItem("Clear Guides", nullptr, false, App::HasDocument()))
                App::Push(Cmd::ViewGuideClearGuides);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void ViewMenu::DrawWindow()
{
}