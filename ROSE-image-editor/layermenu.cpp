#include "layermenu.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"

void LayerMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Layer"))
    {
        const bool doc = App::HasDocument();

        // ---- New ----
        if (ImGui::BeginMenu("New"))
        {
            if (ImGui::MenuItem("Layer...", "Shift+Ctrl+N", false, doc))
                App::Push(Cmd::LayerNewLayer);
            if (ImGui::MenuItem("Layer from Background...", nullptr, false, doc))
                App::Push(Cmd::LayerNewLayerFromBackground);
            if (ImGui::MenuItem("Group...", "Ctrl+G", false, doc))
                App::Push(Cmd::LayerNewGroup);
            if (ImGui::MenuItem("Group from Layers...", nullptr, false, doc))
                App::Push(Cmd::LayerNewGroupFromLayers);
            if (ImGui::MenuItem("Artboard...", nullptr, false, doc))
                App::Push(Cmd::LayerNewArtboard);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Duplicate Layer...", "Ctrl+J", false, doc))
            App::Push(Cmd::LayerDuplicate);

        // ---- Delete ----
        if (ImGui::BeginMenu("Delete"))
        {
            if (ImGui::MenuItem("Layer", "Delete", false, doc))
                App::Push(Cmd::LayerDeleteLayer);
            if (ImGui::MenuItem("Hidden Layers", nullptr, false, doc))
                App::Push(Cmd::LayerDeleteHiddenLayers);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Export ----
        if (ImGui::MenuItem("Quick Export as PNG", nullptr, false, doc))
            App::Push(Cmd::LayerQuickExportPNG);
        if (ImGui::MenuItem("Export As...", nullptr, false, doc))
            App::Push(Cmd::LayerExportAs);

        ImGui::Separator();

        // ---- Layer Style ----
        if (ImGui::BeginMenu("Layer Style"))
        {
            if (ImGui::MenuItem("Blending Options...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleBlendingOptions);
            if (ImGui::MenuItem("Drop Shadow...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleDropShadow);
            if (ImGui::MenuItem("Inner Shadow...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleInnerShadow);
            if (ImGui::MenuItem("Outer Glow...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleOuterGlow);
            if (ImGui::MenuItem("Inner Glow...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleInnerGlow);
            if (ImGui::MenuItem("Bevel and Emboss...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleBevelEmboss);
            if (ImGui::MenuItem("Satin...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleSatin);
            if (ImGui::MenuItem("Color Overlay...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleColorOverlay);
            if (ImGui::MenuItem("Gradient Overlay...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleGradientOverlay);
            if (ImGui::MenuItem("Pattern Overlay...", nullptr, false, doc))
                App::Push(Cmd::LayerStylePatternOverlay);
            if (ImGui::MenuItem("Stroke...", nullptr, false, doc))
                App::Push(Cmd::LayerStyleStroke);

            ImGui::Separator();
            if (ImGui::MenuItem("Copy Layer Style", nullptr, false, doc))
                App::Push(Cmd::LayerStyleCopy);
            if (ImGui::MenuItem("Paste Layer Style", nullptr, false, doc))
                App::Push(Cmd::LayerStylePaste);
            if (ImGui::MenuItem("Clear Layer Style", nullptr, false, doc))
                App::Push(Cmd::LayerStyleClear);
            ImGui::EndMenu();
        }

        // ---- New Fill Layer ----
        if (ImGui::BeginMenu("New Fill Layer"))
        {
            if (ImGui::MenuItem("Solid Color...", nullptr, false, doc))
                App::Push(Cmd::LayerNewFillLayerSolidColor);
            if (ImGui::MenuItem("Gradient...", nullptr, false, doc))
                App::Push(Cmd::LayerNewFillLayerGradient);
            if (ImGui::MenuItem("Pattern...", nullptr, false, doc))
                App::Push(Cmd::LayerNewFillLayerPattern);
            ImGui::EndMenu();
        }

        // ---- New Adjustment Layer ----
        if (ImGui::BeginMenu("New Adjustment Layer"))
        {
            if (ImGui::MenuItem("Levels...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerLevels);
            if (ImGui::MenuItem("Curves...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerCurves);
            if (ImGui::MenuItem("Hue/Saturation...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerHueSaturation);
            if (ImGui::MenuItem("Color Balance...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerColorBalance);
            if (ImGui::MenuItem("Black & White...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerBlackWhite);
            if (ImGui::MenuItem("Channel Mixer...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerChannelMixer);
            if (ImGui::MenuItem("Gradient Map...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerGradientMap);
            if (ImGui::MenuItem("Photo Filter...", nullptr, false, doc))
                App::Push(Cmd::LayerNewAdjustmentLayerPhotoFilter);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Layer Content Options...", nullptr, false, doc))
            App::Push(Cmd::LayerContentOptions);

        ImGui::Separator();

        // ---- Layer Mask ----
        if (ImGui::BeginMenu("Layer Mask"))
        {
            if (ImGui::MenuItem("Reveal All", nullptr, false, doc))
                App::Push(Cmd::LayerMaskRevealAll);
            if (ImGui::MenuItem("Hide All", nullptr, false, doc))
                App::Push(Cmd::LayerMaskHideAll);
            if (ImGui::MenuItem("Reveal Selection", nullptr, false, doc))
                App::Push(Cmd::LayerMaskRevealSelection);
            if (ImGui::MenuItem("Hide Selection", nullptr, false, doc))
                App::Push(Cmd::LayerMaskHideSelection);

            ImGui::Separator();
            if (ImGui::MenuItem("Delete", nullptr, false, doc))
                App::Push(Cmd::LayerMaskDelete);
            if (ImGui::MenuItem("Apply", nullptr, false, doc))
                App::Push(Cmd::LayerMaskApply);
            if (ImGui::MenuItem("Enable", nullptr, false, doc))
                App::Push(Cmd::LayerMaskEnable);
            if (ImGui::MenuItem("Link/Unlink", nullptr, false, doc))
                App::Push(Cmd::LayerMaskLink);
            ImGui::EndMenu();
        }

        // ---- Vector Mask ----
        if (ImGui::BeginMenu("Vector Mask"))
        {
            if (ImGui::MenuItem("Reveal All", nullptr, false, doc))
                App::Push(Cmd::LayerVectorMaskRevealAll);
            if (ImGui::MenuItem("Hide All", nullptr, false, doc))
                App::Push(Cmd::LayerVectorMaskHideAll);
            if (ImGui::MenuItem("Current Path", nullptr, false, doc))
                App::Push(Cmd::LayerVectorMaskCurrentPath);

            ImGui::Separator();
            if (ImGui::MenuItem("Delete", nullptr, false, doc))
                App::Push(Cmd::LayerVectorMaskDelete);
            if (ImGui::MenuItem("Enable", nullptr, false, doc))
                App::Push(Cmd::LayerVectorMaskEnable);
            if (ImGui::MenuItem("Link/Unlink", nullptr, false, doc))
                App::Push(Cmd::LayerVectorMaskLink);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Create Clipping Mask", "Alt+Ctrl+G", false, doc))
            App::Push(Cmd::LayerCreateClippingMask);
        if (ImGui::MenuItem("Release Clipping Mask", "Alt+Ctrl+G", false, doc))
            App::Push(Cmd::LayerReleaseClippingMask);

        ImGui::Separator();

        // ---- Smart Objects / Rasterize ----
        if (ImGui::BeginMenu("Smart Objects"))
        {
            if (ImGui::MenuItem("Convert to Smart Object", nullptr, false, doc))
                App::Push(Cmd::LayerSmartObjectConvert);
            if (ImGui::MenuItem("Rasterize", nullptr, false, doc))
                App::Push(Cmd::LayerSmartObjectRasterize);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Rasterize"))
        {
            if (ImGui::MenuItem("Layer", nullptr, false, doc))
                App::Push(Cmd::LayerRasterizeLayer);
            if (ImGui::MenuItem("Style", nullptr, false, doc))
                App::Push(Cmd::LayerRasterizeStyle);
            if (ImGui::MenuItem("Vector Mask", nullptr, false, doc))
                App::Push(Cmd::LayerRasterizeVectorMask);
            if (ImGui::MenuItem("All Layers", nullptr, false, doc))
                App::Push(Cmd::LayerRasterizeAllLayers);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("New Layer Based on Slice", nullptr, false, doc))
            App::Push(Cmd::LayerNewLayerBasedOnSlice);

        // ---- Group / Ungroup / Hide ----
        if (ImGui::MenuItem("Group Layers", "Ctrl+G", false, doc))
            App::Push(Cmd::LayerGroupLayers);
        if (ImGui::MenuItem("Ungroup Layers", "Shift+Ctrl+G", false, doc))
            App::Push(Cmd::LayerUngroupLayers);
        if (ImGui::MenuItem("Hide Layers", nullptr, false, doc))
            App::Push(Cmd::LayerHideLayers);

        ImGui::Separator();

        // ---- Arrange ----
        if (ImGui::BeginMenu("Arrange"))
        {
            if (ImGui::MenuItem("Bring to Front", "Shift+Ctrl+]", false, doc))
                App::Push(Cmd::LayerArrangeBringToFront);
            if (ImGui::MenuItem("Bring Forward", "Ctrl+]", false, doc))
                App::Push(Cmd::LayerArrangeBringForward);
            if (ImGui::MenuItem("Send Backward", "Ctrl+[", false, doc))
                App::Push(Cmd::LayerArrangeSendBackward);
            if (ImGui::MenuItem("Send to Back", "Shift+Ctrl+[", false, doc))
                App::Push(Cmd::LayerArrangeSendToBack);
            ImGui::Separator();
            if (ImGui::MenuItem("Reverse", nullptr, false, doc))
                App::Push(Cmd::LayerArrangeReverse);
            ImGui::EndMenu();
        }

        // ---- Combine Shapes ----
        if (ImGui::BeginMenu("Combine Shapes"))
        {
            if (ImGui::MenuItem("Unite Shapes", nullptr, false, doc))
                App::Push(Cmd::LayerCombineShapesUnite);
            if (ImGui::MenuItem("Intersect Shapes", nullptr, false, doc))
                App::Push(Cmd::LayerCombineShapesIntersect);
            if (ImGui::MenuItem("Subtract Front Shape", nullptr, false, doc))
                App::Push(Cmd::LayerCombineShapesSubtract);
            if (ImGui::MenuItem("Exclude Overlapping Shapes", nullptr, false, doc))
                App::Push(Cmd::LayerCombineShapesExclude);
            if (ImGui::MenuItem("Merge Shape Components", nullptr, false, doc))
                App::Push(Cmd::LayerCombineShapesMergeShapes);
            ImGui::EndMenu();
        }

        // ---- Align ----
        if (ImGui::BeginMenu("Align"))
        {
            if (ImGui::MenuItem("Top Edges", nullptr, false, doc))
                App::Push(Cmd::LayerAlignTopEdges);
            if (ImGui::MenuItem("Vertical Centers", nullptr, false, doc))
                App::Push(Cmd::LayerAlignVerticalCenters);
            if (ImGui::MenuItem("Bottom Edges", nullptr, false, doc))
                App::Push(Cmd::LayerAlignBottomEdges);
            if (ImGui::MenuItem("Left Edges", nullptr, false, doc))
                App::Push(Cmd::LayerAlignLeftEdges);
            if (ImGui::MenuItem("Horizontal Centers", nullptr, false, doc))
                App::Push(Cmd::LayerAlignHorizontalCenters);
            if (ImGui::MenuItem("Right Edges", nullptr, false, doc))
                App::Push(Cmd::LayerAlignRightEdges);
            ImGui::EndMenu();
        }

        // ---- Distribute ----
        if (ImGui::BeginMenu("Distribute"))
        {
            if (ImGui::MenuItem("Top Edges", nullptr, false, doc))
                App::Push(Cmd::LayerDistributeTopEdges);
            if (ImGui::MenuItem("Vertical Centers", nullptr, false, doc))
                App::Push(Cmd::LayerDistributeVerticalCenters);
            if (ImGui::MenuItem("Bottom Edges", nullptr, false, doc))
                App::Push(Cmd::LayerDistributeBottomEdges);
            if (ImGui::MenuItem("Left Edges", nullptr, false, doc))
                App::Push(Cmd::LayerDistributeLeftEdges);
            if (ImGui::MenuItem("Horizontal Centers", nullptr, false, doc))
                App::Push(Cmd::LayerDistributeHorizontalCenters);
            if (ImGui::MenuItem("Right Edges", nullptr, false, doc))
                App::Push(Cmd::LayerDistributeRightEdges);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Lock Layers...", nullptr, false, doc))
            App::Push(Cmd::LayerLockLayers);

        ImGui::Separator();

        // ---- Merge / Flatten ----
        if (ImGui::MenuItem("Merge Layers", "Ctrl+E", false, doc))
            App::Push(Cmd::LayerMergeDown);
        if (ImGui::MenuItem("Merge Visible", "Shift+Ctrl+E", false, doc))
            App::Push(Cmd::LayerMergeVisible);
        if (ImGui::MenuItem("Flatten Image", nullptr, false, doc))
            App::Push(Cmd::LayerFlatten);

        // ---- Matting ----
        if (ImGui::BeginMenu("Matting"))
        {
            if (ImGui::MenuItem("Defringe...", nullptr, false, doc))
                App::Push(Cmd::LayerMattingDefringe);
            if (ImGui::MenuItem("Remove Black Matte", nullptr, false, doc))
                App::Push(Cmd::LayerMattingRemoveBlackMatte);
            if (ImGui::MenuItem("Remove White Matte", nullptr, false, doc))
                App::Push(Cmd::LayerMattingRemoveWhiteMatte);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void LayerMenu::DrawWindow()
{
}