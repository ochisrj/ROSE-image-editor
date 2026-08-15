#include "editmenu.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"

void EditMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Edit"))
    {
        const bool doc = App::HasDocument();

        // ---- Undo / Redo ----
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, doc))
            App::Push(Cmd::EditUndo);
        if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", false, doc))
            App::Push(Cmd::EditRedo);
        if (ImGui::MenuItem("Toggle Last State", "Alt+Ctrl+Z", false, doc))
            App::Push(Cmd::EditToggleLastState);

        ImGui::Separator();

        if (ImGui::MenuItem("Fade...", "Shift+Ctrl+F", false, doc))
            App::Push(Cmd::EditFade);

        ImGui::Separator();

        // ---- Clipboard ----
        if (ImGui::MenuItem("Cut", "Ctrl+X", false, doc))
            App::Push(Cmd::EditCut);
        if (ImGui::MenuItem("Copy", "Ctrl+C", false, doc))
            App::Push(Cmd::EditCopy);
        if (ImGui::MenuItem("Copy Merged", "Shift+Ctrl+C", false, doc))
            App::Push(Cmd::EditCopyMerged);
        if (ImGui::MenuItem("Paste", "Ctrl+V"))
            App::Push(Cmd::EditPaste);

        if (ImGui::BeginMenu("Paste Special"))
        {
            if (ImGui::MenuItem("Paste in Place", "Shift+Ctrl+V"))
                App::Push(Cmd::EditPasteInPlace);
            if (ImGui::MenuItem("Paste Into", "Alt+Shift+Ctrl+V"))
                App::Push(Cmd::EditPasteInto);
            if (ImGui::MenuItem("Paste Outside"))
                App::Push(Cmd::EditPasteOutside);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Clear", nullptr, false, doc))
            App::Push(Cmd::EditClear);

        ImGui::Separator();

        // ---- Search / Spelling ----
        if (ImGui::MenuItem("Search", "Ctrl+F"))
            App::Push(Cmd::EditSearch);

        ImGui::Separator();

        if (ImGui::MenuItem("Check Spelling...", nullptr, false, doc))
            App::Push(Cmd::EditCheckSpelling);
        if (ImGui::MenuItem("Find and Replace Text...", nullptr, false, doc))
            App::Push(Cmd::EditFindAndReplaceText);

        ImGui::Separator();

        // ---- Fill / Stroke ----
        if (ImGui::MenuItem("Fill...", "Shift+F5", false, doc))
            App::Push(Cmd::EditFill);
        if (ImGui::MenuItem("Stroke...", nullptr, false, doc))
            App::Push(Cmd::EditStroke);
        if (ImGui::MenuItem("Content-Aware Fill...", nullptr, false, doc))
            App::Push(Cmd::EditContentAwareFill);
        if (ImGui::MenuItem("Content-Aware Scale", "Alt+Shift+Ctrl+C", false, doc))
            App::Push(Cmd::EditContentAwareScale);

        ImGui::Separator();

        // ---- Transform ----
        if (ImGui::MenuItem("Free Transform", "Ctrl+T", false, doc))
            App::Push(Cmd::EditFreeTransform);

        if (ImGui::BeginMenu("Transform"))
        {
            if (ImGui::MenuItem("Again", "Shift+Ctrl+T", false, doc))
                App::Push(Cmd::EditTransformAgain);
            if (ImGui::MenuItem("Scale", nullptr, false, doc))
                App::Push(Cmd::EditTransformScale);
            if (ImGui::MenuItem("Rotate", nullptr, false, doc))
                App::Push(Cmd::EditTransformRotate);
            if (ImGui::MenuItem("Skew", nullptr, false, doc))
                App::Push(Cmd::EditTransformSkew);
            if (ImGui::MenuItem("Distort", nullptr, false, doc))
                App::Push(Cmd::EditTransformDistort);
            if (ImGui::MenuItem("Perspective", nullptr, false, doc))
                App::Push(Cmd::EditTransformPerspective);
            if (ImGui::MenuItem("Warp", nullptr, false, doc))
                App::Push(Cmd::EditTransformWarp);
            if (ImGui::MenuItem("Split Warp Horizontally", nullptr, false, doc))
                App::Push(Cmd::EditTransformSplitWarpHorizontally);
            if (ImGui::MenuItem("Split Warp Vertically", nullptr, false, doc))
                App::Push(Cmd::EditTransformSplitWarpVertically);

            ImGui::Separator();
            if (ImGui::MenuItem("Rotate 180\xC2\xB0", nullptr, false, doc))
                App::Push(Cmd::EditTransformRotate180);
            if (ImGui::MenuItem("Rotate 90\xC2\xB0 CW", nullptr, false, doc))
                App::Push(Cmd::EditTransformRotate90CW);
            if (ImGui::MenuItem("Rotate 90\xC2\xB0 CCW", nullptr, false, doc))
                App::Push(Cmd::EditTransformRotate90CCW);
            if (ImGui::MenuItem("Flip Horizontal", nullptr, false, doc))
                App::Push(Cmd::EditTransformFlipHorizontal);
            if (ImGui::MenuItem("Flip Vertical", nullptr, false, doc))
                App::Push(Cmd::EditTransformFlipVertical);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Auto-Align Layers...", nullptr, false, doc))
            App::Push(Cmd::EditAutoAlignLayers);
        if (ImGui::MenuItem("Auto-Blend Layers...", nullptr, false, doc))
            App::Push(Cmd::EditAutoBlendLayers);

        ImGui::Separator();

        // ---- Define Preset ----
        if (ImGui::BeginMenu("Define Preset"))
        {
            if (ImGui::MenuItem("Define Brush Preset..."))
                App::Push(Cmd::EditDefineBrushPreset);
            if (ImGui::MenuItem("Define Pattern Preset..."))
                App::Push(Cmd::EditDefinePatternPreset);
            if (ImGui::MenuItem("Define Custom Shape Preset..."))
                App::Push(Cmd::EditDefineCustomShapePreset);
            ImGui::EndMenu();
        }

        // ---- Purge ----
        if (ImGui::BeginMenu("Purge"))
        {
            if (ImGui::MenuItem("Clipboard"))
                App::Push(Cmd::EditPurgeClipboard);
            if (ImGui::MenuItem("Histories"))
                App::Push(Cmd::EditPurgeHistories);
            if (ImGui::MenuItem("All"))
                App::Push(Cmd::EditPurgeAll);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Color Settings / Shortcuts / Menus ----
        if (ImGui::MenuItem("Color Settings...", "Shift+Ctrl+K"))
            App::Push(Cmd::EditColorSettings);
        if (ImGui::MenuItem("Keyboard Shortcuts...", "Alt+Shift+Ctrl+K"))
            App::Push(Cmd::EditKeyboardShortcuts);
        if (ImGui::MenuItem("Menus...", "Alt+Shift+Ctrl+M"))
            App::Push(Cmd::EditMenus);

        // ---- Preferences ----
        if (ImGui::BeginMenu("Preferences"))
        {
            if (ImGui::MenuItem("General...", "Ctrl+K"))
                App::Push(Cmd::EditPreferencesGeneral);
            if (ImGui::MenuItem("Performance..."))
                App::Push(Cmd::EditPreferencesPerformance);
            if (ImGui::MenuItem("Scratch Disks..."))
                App::Push(Cmd::EditPreferencesScratchDisks);
            if (ImGui::MenuItem("Display & Interface..."))
                App::Push(Cmd::EditPreferencesDisplay);
            if (ImGui::MenuItem("Units & Rulers..."))
                App::Push(Cmd::EditPreferencesUnitsRulers);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void EditMenu::DrawWindow()
{
    if (App::ShowPreferences)
    {
        ImGui::OpenPopup("Preferences");
        App::ShowPreferences = false;
    }

    bool open = true;
    if (ImGui::BeginPopupModal("Preferences", &open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        if (ImGui::BeginTabBar("##prefs_tabs"))
        {
            if (ImGui::BeginTabItem("General"))
            {
                ImGui::Checkbox("Show FPS in menu bar", &App::PrefShowFps);
                ImGui::Checkbox("VSync", &App::PrefVSync);
                ImGui::Checkbox("Dark theme", &App::PrefThemeDark);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Performance"))
            {
                ImGui::Checkbox("Show FPS in menu bar", &App::PrefShowFps);
                ImGui::TextDisabled("FPS / frame-time overlay");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Scratch Disks"))
            {
                ImGui::TextDisabled("Scratch disk configuration (template stub)");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Display / Interface"))
            {
                ImGui::Checkbox("Show grid", &App::ShowGrid);
                ImGui::Checkbox("Show rulers", &App::ShowRulers);
                ImGui::Checkbox("Show guides", &App::ShowGuides);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Units & Rulers"))
            {
                ImGui::TextDisabled("Units / ruler configuration (template stub)");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(120, 0)))
            open = false;
        ImGui::EndPopup();
    }
}