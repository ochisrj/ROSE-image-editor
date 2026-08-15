#include "selectmenu.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"

void SelectMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Select"))
    {
        const bool doc = App::HasDocument();

        // ---- Basic selection ----
        if (ImGui::MenuItem("All", "Ctrl+A", false, doc))
            App::Push(Cmd::SelectAll);
        if (ImGui::MenuItem("Deselect", "Ctrl+D", false, doc))
            App::Push(Cmd::SelectDeselect);
        if (ImGui::MenuItem("Reselect", "Shift+Ctrl+D", false, doc))
            App::Push(Cmd::SelectReselect);
        if (ImGui::MenuItem("Inverse", "Shift+Ctrl+I", false, doc))
            App::Push(Cmd::SelectInverse);

        ImGui::Separator();

        // ---- Layer selection ----
        if (ImGui::MenuItem("All Layers", "Alt+Ctrl+A", false, doc))
            App::Push(Cmd::SelectAllLayers);
        if (ImGui::MenuItem("Deselect Layers", nullptr, false, doc))
            App::Push(Cmd::SelectDeselectLayers);
        if (ImGui::MenuItem("Find Layers", "Alt+Shift+Ctrl+F", false, doc))
            App::Push(Cmd::SelectFindLayers);
        if (ImGui::MenuItem("Isolate Layers", nullptr, false, doc))
            App::Push(Cmd::SelectIsolateLayers);

        ImGui::Separator();

        // ---- Color / Focus ----
        if (ImGui::MenuItem("Color Range...", nullptr, false, doc))
            App::Push(Cmd::SelectColorRange);
        if (ImGui::MenuItem("Focus Area...", nullptr, false, doc))
            App::Push(Cmd::SelectFocusArea);
        if (ImGui::MenuItem("Subject", nullptr, false, doc))
            App::Push(Cmd::SelectSubject);
        if (ImGui::MenuItem("Sky", nullptr, false, doc))
            App::Push(Cmd::SelectSky);

        ImGui::Separator();

        if (ImGui::MenuItem("Select and Mask...", "Alt+Ctrl+R", false, doc))
            App::Push(Cmd::SelectAndMask);

        ImGui::Separator();

        // ---- Modify ----
        if (ImGui::BeginMenu("Modify"))
        {
            if (ImGui::MenuItem("Border...", nullptr, false, doc))
                App::Push(Cmd::SelectModifyBorder);
            if (ImGui::MenuItem("Smooth...", nullptr, false, doc))
                App::Push(Cmd::SelectModifySmooth);
            if (ImGui::MenuItem("Expand...", nullptr, false, doc))
                App::Push(Cmd::SelectModifyExpand);
            if (ImGui::MenuItem("Contract...", nullptr, false, doc))
                App::Push(Cmd::SelectModifyContract);
            if (ImGui::MenuItem("Feather...", "Shift+F6", false, doc))
                App::Push(Cmd::SelectModifyFeather);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Grow", nullptr, false, doc))
            App::Push(Cmd::SelectGrow);
        if (ImGui::MenuItem("Similar", nullptr, false, doc))
            App::Push(Cmd::SelectSimilar);

        ImGui::Separator();

        if (ImGui::MenuItem("Transform Selection", nullptr, false, doc))
            App::Push(Cmd::SelectTransformSelection);
        if (ImGui::MenuItem("Edit in Quick Mask Mode", "Q", false, doc))
            App::Push(Cmd::SelectQuickMaskMode);

        ImGui::Separator();

        if (ImGui::MenuItem("Save Selection...", nullptr, false, doc))
            App::Push(Cmd::SelectSaveSelection);
        if (ImGui::MenuItem("Load Selection...", nullptr, false, doc))
            App::Push(Cmd::SelectLoadSelection);

        ImGui::EndMenu();
    }
}

void SelectMenu::DrawWindow()
{
}