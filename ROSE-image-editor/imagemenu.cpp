#include "imagemenu.h"

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"

void ImageMenu::DrawMenu()
{
    if (ImGui::BeginMenu("Image"))
    {
        const bool doc = App::HasDocument();

        // ---- Mode ----
        if (ImGui::BeginMenu("Mode"))
        {
            static int mode = 0;       // 0 RGB, 1 Grayscale, ...
            static int bits = 0;       // 0 = 8, 1 = 16, 2 = 32

            if (ImGui::MenuItem("Bitmap", nullptr, mode == 0))
            { mode = 0; App::Push(Cmd::ImageModeBitmap); }
            if (ImGui::MenuItem("Grayscale", nullptr, mode == 1))
            { mode = 1; App::Push(Cmd::ImageModeGrayscale); }
            if (ImGui::MenuItem("Duotone", nullptr, mode == 2, false))
            { mode = 2; App::Push(Cmd::ImageModeDuotone); }
            if (ImGui::MenuItem("Indexed Color...", nullptr, mode == 3, false))
            { mode = 3; App::Push(Cmd::ImageModeIndexed); }
            if (ImGui::MenuItem("RGB Color", nullptr, mode == 4))
            { mode = 4; App::Push(Cmd::ImageModeRGB); }
            if (ImGui::MenuItem("CMYK Color", nullptr, mode == 5))
            { mode = 5; App::Push(Cmd::ImageModeCMYK); }
            if (ImGui::MenuItem("Lab Color", nullptr, mode == 6))
            { mode = 6; App::Push(Cmd::ImageModeLab); }
            if (ImGui::MenuItem("Multichannel", nullptr, mode == 7, false))
            { mode = 7; App::Push(Cmd::ImageModeMultichannel); }

            ImGui::Separator();
            if (ImGui::MenuItem("8 Bits/Channel", nullptr, bits == 0))
            { bits = 0; App::Push(Cmd::ImageBits8); }
            if (ImGui::MenuItem("16 Bits/Channel", nullptr, bits == 1))
            { bits = 1; App::Push(Cmd::ImageBits16); }
            if (ImGui::MenuItem("32 Bits/Channel", nullptr, bits == 2, false))
            { bits = 2; App::Push(Cmd::ImageBits32); }
            ImGui::EndMenu();
        }

        // ---- Adjustments ----
        if (ImGui::BeginMenu("Adjustments"))
        {
            if (ImGui::MenuItem("Brightness/Contrast...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustBrightnessContrast);
            if (ImGui::MenuItem("Levels...", "Ctrl+L", false, doc))
                App::Push(Cmd::ImageAdjustLevels);
            if (ImGui::MenuItem("Curves...", "Ctrl+M", false, doc))
                App::Push(Cmd::ImageAdjustCurves);
            if (ImGui::MenuItem("Exposure...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustExposure);
            if (ImGui::MenuItem("Vibrance...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustVibrance);
            if (ImGui::MenuItem("Hue/Saturation...", "Ctrl+U", false, doc))
                App::Push(Cmd::ImageAdjustHueSaturation);
            if (ImGui::MenuItem("Color Balance...", "Ctrl+B", false, doc))
                App::Push(Cmd::ImageAdjustColorBalance);
            if (ImGui::MenuItem("Black & White...", "Alt+Shift+Ctrl+B", false, doc))
                App::Push(Cmd::ImageAdjustBlackWhite);
            if (ImGui::MenuItem("Photo Filter...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustPhotoFilter);
            if (ImGui::MenuItem("Channel Mixer...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustChannelMixer);
            if (ImGui::MenuItem("Color Lookup...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustColorLookup);

            ImGui::Separator();
            if (ImGui::MenuItem("Invert", "Ctrl+I", false, doc))
                App::Push(Cmd::ImageAdjustInvert);
            if (ImGui::MenuItem("Posterize...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustPosterize);
            if (ImGui::MenuItem("Threshold...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustThreshold);
            if (ImGui::MenuItem("Gradient Map...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustGradientMap);
            if (ImGui::MenuItem("Selective Color...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustSelectiveColor);

            ImGui::Separator();
            if (ImGui::MenuItem("Shadows/Highlights...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustShadowsHighlights);
            if (ImGui::MenuItem("HDR Toning...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustHDRToning);

            ImGui::Separator();
            if (ImGui::MenuItem("Desaturate", "Shift+Ctrl+U", false, doc))
                App::Push(Cmd::ImageAdjustDesaturate);
            if (ImGui::MenuItem("Match Color...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustMatchColor);
            if (ImGui::MenuItem("Replace Color...", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustReplaceColor);
            if (ImGui::MenuItem("Equalize", nullptr, false, doc))
                App::Push(Cmd::ImageAdjustEqualize);
            ImGui::EndMenu();
        }

        // ---- Auto Tone / Contrast / Color ----
        if (ImGui::MenuItem("Auto Tone", "Shift+Ctrl+N", false, doc))
            App::Push(Cmd::ImageAutoTone);
        if (ImGui::MenuItem("Auto Contrast", "Alt+Shift+Ctrl+N", false, doc))
            App::Push(Cmd::ImageAutoContrast);
        if (ImGui::MenuItem("Auto Color", "Shift+Ctrl+B", false, doc))
            App::Push(Cmd::ImageAutoColor);

        ImGui::Separator();

        // ---- Image / Canvas Size ----
        if (ImGui::MenuItem("Image Size...", "Alt+Ctrl+I", false, doc))
            App::Push(Cmd::ImageSize);
        if (ImGui::MenuItem("Canvas Size...", "Alt+Ctrl+C", false, doc))
            App::Push(Cmd::CanvasSize);

        // ---- Image Rotation ----
        if (ImGui::BeginMenu("Image Rotation"))
        {
            if (ImGui::MenuItem("180\xC2\xB0", nullptr, false, doc))
                App::Push(Cmd::ImageRotate180);
            if (ImGui::MenuItem("90\xC2\xB0 CW", nullptr, false, doc))
                App::Push(Cmd::ImageRotate90CW);
            if (ImGui::MenuItem("90\xC2\xB0 CCW", nullptr, false, doc))
                App::Push(Cmd::ImageRotate90CCW);
            if (ImGui::MenuItem("Arbitrary...", nullptr, false, doc))
                App::Push(Cmd::ImageRotateArbitrary);
            ImGui::Separator();
            if (ImGui::MenuItem("Flip Canvas Horizontal", nullptr, false, doc))
                App::Push(Cmd::ImageFlipHorizontal);
            if (ImGui::MenuItem("Flip Canvas Vertical", nullptr, false, doc))
                App::Push(Cmd::ImageFlipVertical);
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Crop", nullptr, false, doc))
            App::Push(Cmd::ImageCrop);
        if (ImGui::MenuItem("Trim...", nullptr, false, doc))
            App::Push(Cmd::ImageTrim);
        if (ImGui::MenuItem("Reveal All", nullptr, false, doc))
            App::Push(Cmd::ImageRevealAll);

        ImGui::Separator();

        // ---- Duplicate / Apply Image / Calculations ----
        if (ImGui::MenuItem("Duplicate...", nullptr, false, doc))
            App::Push(Cmd::ImageDuplicate);
        if (ImGui::MenuItem("Apply Image...", nullptr, false, doc))
            App::Push(Cmd::ImageApplyImage);
        if (ImGui::MenuItem("Calculations...", nullptr, false, doc))
            App::Push(Cmd::ImageCalculations);

        // ---- Variables / Analysis ----
        if (ImGui::BeginMenu("Variables"))
        {
            if (ImGui::MenuItem("Data Sets..."))
                App::Push(Cmd::ImageVariablesDataSets);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Analysis"))
        {
            if (ImGui::MenuItem("Scale Marker..."))
                App::Push(Cmd::ImageAnalysisScaleMarker);
            if (ImGui::MenuItem("Ruler Tool Metrics"))
                App::Push(Cmd::ImageAnalysisRulerTool);
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void ImageMenu::DrawWindow()
{
}