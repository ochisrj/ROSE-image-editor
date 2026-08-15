#include "filtermenu.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"

void FilterMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Filter"))
    {
        const bool doc = App::HasDocument();

        // ---- Last Filter / Smart Filters ----
        if (ImGui::MenuItem("Last Filter", "Ctrl+F", false, doc))
            App::Push(Cmd::FilterLastFilter);
        if (ImGui::MenuItem("Convert for Smart Filters", nullptr, false, doc))
            App::Push(Cmd::FilterConvertForSmartFilters);

        ImGui::Separator();

        // ---- Main filter dialogs ----
        if (ImGui::MenuItem("Filter Gallery...", nullptr, false, doc))
            App::Push(Cmd::FilterGallery);
        if (ImGui::MenuItem("Adaptive Wide Angle...", "Alt+Shift+Ctrl+A", false, doc))
            App::Push(Cmd::FilterAdaptiveWideAngle);
        if (ImGui::MenuItem("Camera Raw Filter...", "Shift+Ctrl+A", false, doc))
            App::Push(Cmd::FilterCameraRaw);
        if (ImGui::MenuItem("Lens Correction...", "Shift+Ctrl+R", false, doc))
            App::Push(Cmd::FilterLensCorrection);
        if (ImGui::MenuItem("Liquify...", "Shift+Ctrl+X", false, doc))
            App::Push(Cmd::FilterLiquify);
        if (ImGui::MenuItem("Vanish Point...", "Alt+Ctrl+V", false, doc))
            App::Push(Cmd::FilterVanishPoint);

        ImGui::Separator();

        // ---- 3D ----
        if (ImGui::BeginMenu("3D"))
        {
            if (ImGui::MenuItem("Generate Bump Map...", nullptr, false, doc))
                App::Push(Cmd::Filter3DGenerateBumpMap);
            if (ImGui::MenuItem("Generate Normal Map...", nullptr, false, doc))
                App::Push(Cmd::Filter3DGenerateNormalMap);
            ImGui::EndMenu();
        }

        // ---- Blur ----
        if (ImGui::BeginMenu("Blur"))
        {
            if (ImGui::MenuItem("Average", nullptr, false, doc))
                App::Push(Cmd::FilterBlurAverage);
            if (ImGui::MenuItem("Blur", nullptr, false, doc))
                App::Push(Cmd::FilterBlurBlur);
            if (ImGui::MenuItem("Blur More", nullptr, false, doc))
                App::Push(Cmd::FilterBlurBlurMore);
            if (ImGui::MenuItem("Box Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurBoxBlur);
            if (ImGui::MenuItem("Gaussian Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurGaussianBlur);
            if (ImGui::MenuItem("Lens Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurLensBlur);
            if (ImGui::MenuItem("Motion Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurMotionBlur);
            if (ImGui::MenuItem("Radial Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurRadialBlur);
            if (ImGui::MenuItem("Surface Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurSurfaceBlur);
            ImGui::EndMenu();
        }

        // ---- Blur Gallery ----
        if (ImGui::BeginMenu("Blur Gallery"))
        {
            if (ImGui::MenuItem("Field Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurGalleryFieldBlur);
            if (ImGui::MenuItem("Iris Blur...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurGalleryIrisBlur);
            if (ImGui::MenuItem("Tilt-Shift...", nullptr, false, doc))
                App::Push(Cmd::FilterBlurGalleryTiltShift);
            ImGui::EndMenu();
        }

        // ---- Distort ----
        if (ImGui::BeginMenu("Distort"))
        {
            if (ImGui::MenuItem("Displace...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortDisplace);
            if (ImGui::MenuItem("Pinch...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortPinch);
            if (ImGui::MenuItem("Polar Coordinates...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortPolarCoordinates);
            if (ImGui::MenuItem("Ripple...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortRipple);
            if (ImGui::MenuItem("Shear...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortShear);
            if (ImGui::MenuItem("Spherize...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortSpherize);
            if (ImGui::MenuItem("Twirl...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortTwirl);
            if (ImGui::MenuItem("Wave...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortWave);
            if (ImGui::MenuItem("ZigZag...", nullptr, false, doc))
                App::Push(Cmd::FilterDistortZigZag);
            ImGui::EndMenu();
        }

        // ---- Noise ----
        if (ImGui::BeginMenu("Noise"))
        {
            if (ImGui::MenuItem("Add Noise...", nullptr, false, doc))
                App::Push(Cmd::FilterNoiseAddNoise);
            if (ImGui::MenuItem("Despeckle", nullptr, false, doc))
                App::Push(Cmd::FilterNoiseDespeckle);
            if (ImGui::MenuItem("Dust & Scratches...", nullptr, false, doc))
                App::Push(Cmd::FilterNoiseDustScratches);
            if (ImGui::MenuItem("Median...", nullptr, false, doc))
                App::Push(Cmd::FilterNoiseMedian);
            if (ImGui::MenuItem("Reduce Noise...", nullptr, false, doc))
                App::Push(Cmd::FilterNoiseReduceNoise);
            ImGui::EndMenu();
        }

        // ---- Pixelate ----
        if (ImGui::BeginMenu("Pixelate"))
        {
            if (ImGui::MenuItem("Color Halftone...", nullptr, false, doc))
                App::Push(Cmd::FilterPixelateColorHalftone);
            if (ImGui::MenuItem("Crystallize...", nullptr, false, doc))
                App::Push(Cmd::FilterPixelateCrystallize);
            if (ImGui::MenuItem("Facet", nullptr, false, doc))
                App::Push(Cmd::FilterPixelateFacet);
            if (ImGui::MenuItem("Mosaic...", nullptr, false, doc))
                App::Push(Cmd::FilterPixelateMosaic);
            if (ImGui::MenuItem("Mezzotint...", nullptr, false, doc))
                App::Push(Cmd::FilterPixelateMezzotint);
            if (ImGui::MenuItem("Pointillize...", nullptr, false, doc))
                App::Push(Cmd::FilterPixelatePointillize);
            ImGui::EndMenu();
        }

        // ---- Render ----
        if (ImGui::BeginMenu("Render"))
        {
            if (ImGui::MenuItem("Clouds", nullptr, false, doc))
                App::Push(Cmd::FilterRenderClouds);
            if (ImGui::MenuItem("Difference Clouds", nullptr, false, doc))
                App::Push(Cmd::FilterRenderDifferenceClouds);
            if (ImGui::MenuItem("Fibers...", nullptr, false, doc))
                App::Push(Cmd::FilterRenderFibers);
            if (ImGui::MenuItem("Lens Flare...", nullptr, false, doc))
                App::Push(Cmd::FilterRenderLensFlare);
            if (ImGui::MenuItem("Lighting Effects...", nullptr, false, doc))
                App::Push(Cmd::FilterRenderLightingEffects);
            ImGui::EndMenu();
        }

        // ---- Sharpen ----
        if (ImGui::BeginMenu("Sharpen"))
        {
            if (ImGui::MenuItem("Sharpen", nullptr, false, doc))
                App::Push(Cmd::FilterSharpenSharpen);
            if (ImGui::MenuItem("Sharpen Edges", nullptr, false, doc))
                App::Push(Cmd::FilterSharpenSharpenEdges);
            if (ImGui::MenuItem("Sharpen More", nullptr, false, doc))
                App::Push(Cmd::FilterSharpenSharpenMore);
            if (ImGui::MenuItem("Smart Sharpen...", nullptr, false, doc))
                App::Push(Cmd::FilterSharpenSmartSharpen);
            if (ImGui::MenuItem("Unsharp Mask...", nullptr, false, doc))
                App::Push(Cmd::FilterSharpenUnsharpMask);
            ImGui::EndMenu();
        }

        // ---- Stylize ----
        if (ImGui::BeginMenu("Stylize"))
        {
            if (ImGui::MenuItem("Diffuse...", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeDiffuse);
            if (ImGui::MenuItem("Emboss...", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeEmboss);
            if (ImGui::MenuItem("Extrude...", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeExtrude);
            if (ImGui::MenuItem("Find Edges", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeFindEdges);
            if (ImGui::MenuItem("Oil Paint...", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeOilPaint);
            if (ImGui::MenuItem("Solarize", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeSolarize);
            if (ImGui::MenuItem("Tiles...", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeTiles);
            if (ImGui::MenuItem("Trace Contour...", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeTraceContour);
            if (ImGui::MenuItem("Wind...", nullptr, false, doc))
                App::Push(Cmd::FilterStylizeWind);
            ImGui::EndMenu();
        }

        // ---- Other ----
        if (ImGui::BeginMenu("Other"))
        {
            if (ImGui::MenuItem("Custom...", nullptr, false, doc))
                App::Push(Cmd::FilterOtherCustom);
            if (ImGui::MenuItem("High Pass...", nullptr, false, doc))
                App::Push(Cmd::FilterOtherHighPass);
            if (ImGui::MenuItem("Maximum...", nullptr, false, doc))
                App::Push(Cmd::FilterOtherMaximum);
            if (ImGui::MenuItem("Minimum...", nullptr, false, doc))
                App::Push(Cmd::FilterOtherMinimum);
            if (ImGui::MenuItem("Offset...", nullptr, false, doc))
                App::Push(Cmd::FilterOtherOffset);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void FilterMenu::DrawWindow()
{
}