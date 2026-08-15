#include "appstate.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imageviewer.h"

namespace App
{
    // ---- Shared UI state ----
    bool ShowTools       = false;
    bool ShowLayers      = false;
    bool ShowColor       = false;
    bool ShowHistory     = false;
    bool ShowProperties  = false;
    bool ShowViewport    = true;
    bool Show3D          = false;
    bool ShowActions     = false;
    bool ShowAdjustments = false;
    bool ShowBrushSettings = false;
    bool ShowBrushes     = false;
    bool ShowChannels    = false;
    bool ShowCharacter   = false;
    bool ShowNavigator   = false;
    bool ShowParagraph   = false;
    bool ShowPaths       = false;
    bool ShowSwatches    = false;
    bool ShowTimeline    = false;
    bool ShowToolSettings = false;
    bool ShowInfo        = false;

    bool ShowGrid        = false;
    bool ShowRulers      = false;
    bool ShowGuides      = false;
    bool ShowTargetPath  = false;
    bool ShowCanvasGuides = false;
    bool ShowPixelGrid   = false;
    bool ShowLayerEdges  = false;
    bool ShowSelectionEdges = false;

    int   ScreenMode     = 0;
    bool  ShowProofColors   = false;
    bool  ShowGamutWarning  = false;
    bool  SnapEnabled       = true;
    bool  SnapToGuides      = true;
    bool  SnapToGrid        = false;
    bool  SnapToLayers      = true;
    bool  SnapToSlices      = false;
    bool  SnapToDocumentBounds = true;
    bool  GuidesLocked      = false;
    bool  ApplicationFrame  = true;
    bool  ShowOptions       = true;

    bool ShowNewDocument         = false;
    bool ShowPreferences         = false;
    bool ShowKeyboardShortcuts   = false;
    bool ShowAbout               = false;

    int   PrefTab      = 0;
    bool  PrefThemeDark = true;
    bool  PrefVSync    = true;
    bool  PrefShowFps  = true;

    std::vector<std::string> RecentFiles;
    std::string Status;

    static GLFWwindow*           s_Window = nullptr;
    static std::vector<Cmd>      s_PendingCommands;

    void SetWindow(GLFWwindow* window) { s_Window = window; }
    GLFWwindow* Window() { return s_Window; }

    void SetStatus(const char* fmt, ...)
    {
        char buf[256];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        Status = buf;
        if (Status.size() > 72)
            Status.resize(72);
    }

    void AddRecent(const std::string& path)
    {
        if (path.empty())
            return;
        auto it = std::find(RecentFiles.begin(), RecentFiles.end(), path);
        if (it != RecentFiles.end())
            RecentFiles.erase(it);
        RecentFiles.insert(RecentFiles.begin(), path);
        const int kMaxRecent = 10;
        if ((int)RecentFiles.size() > kMaxRecent)
            RecentFiles.resize(kMaxRecent);
    }

    void Push(Cmd cmd) { s_PendingCommands.push_back(cmd); }

    bool HasDocument()   { return ImageViewer::HasImage(); }
    bool HasSelection()  { return false; }

    // MUST stay in the exact same order as the Cmd enum in appcommands.h.
    // The static_assert below enforces this.
    static const char* const s_CmdNames[] =
    {
        // ---- File ----
        "File > New", "File > Open", "File > Browse in Bridge", "File > Open As",
        "File > Open as Smart Object",
        "File > Close", "File > Close All", "File > Close and Go to Bridge",
        "File > Save", "File > Save As", "File > Save a Copy", "File > Revert",
        "File > Quick Export as PNG", "File > Export As", "File > Export Preferences",
        "File > Save for Web (Legacy)", "File > Render Video", "File > Generate > Image Assets",
        "File > Place Embedded", "File > Place Linked", "File > Package",
        "File > Automate > Batch", "File > Automate > Create Droplet",
        "File > Automate > Crop and Straighten Photos", "File > Automate > Contact Sheet II",
        "File > Automate > PDF Presentation", "File > Automate > Photomerge",
        "File > Scripts > Image Processor", "File > Scripts > Delete All Empty Layers",
        "File > Scripts > Load Files into Stack", "File > Scripts > Browse",
        "File > Print", "File > Print One Copy", "File > Exit",

        // ---- Edit ----
        "Edit > Undo", "Edit > Redo", "Edit > Toggle Last State", "Edit > Fade",
        "Edit > Cut", "Edit > Copy", "Edit > Copy Merged", "Edit > Paste",
        "Edit > Paste Special > Paste in Place", "Edit > Paste Special > Paste Into",
        "Edit > Paste Special > Paste Outside", "Edit > Clear", "Edit > Search",
        "Edit > Check Spelling", "Edit > Find and Replace Text",
        "Edit > Fill", "Edit > Stroke", "Edit > Content-Aware Fill", "Edit > Content-Aware Scale",
        "Edit > Free Transform",
        "Edit > Transform > Again", "Edit > Transform > Scale", "Edit > Transform > Rotate",
        "Edit > Transform > Skew", "Edit > Transform > Distort", "Edit > Transform > Perspective",
        "Edit > Transform > Warp", "Edit > Transform > Split Warp Horizontally",
        "Edit > Transform > Split Warp Vertically", "Edit > Transform > Rotate 180",
        "Edit > Transform > Rotate 90 CW", "Edit > Transform > Rotate 90 CCW",
        "Edit > Transform > Flip Horizontal", "Edit > Transform > Flip Vertical",
        "Edit > Auto-Align Layers", "Edit > Auto-Blend Layers",
        "Edit > Define Brush Preset", "Edit > Define Pattern Preset", "Edit > Define Custom Shape Preset",
        "Edit > Purge > Clipboard", "Edit > Purge > Histories", "Edit > Purge > All",
        "Edit > Color Settings", "Edit > Keyboard Shortcuts", "Edit > Menus",
        "Edit > Preferences > General", "Edit > Preferences > Performance",
        "Edit > Preferences > Scratch Disks", "Edit > Preferences > Display & Interface",
        "Edit > Preferences > Units & Rulers",

        // ---- Image ----
        "Image > Mode > Bitmap", "Image > Mode > Grayscale", "Image > Mode > Duotone",
        "Image > Mode > Indexed Color", "Image > Mode > RGB Color", "Image > Mode > CMYK Color",
        "Image > Mode > Lab Color", "Image > Mode > Multichannel",
        "Image > Mode > 8 Bits/Channel", "Image > Mode > 16 Bits/Channel", "Image > Mode > 32 Bits/Channel",
        "Image > Adjustments > Brightness/Contrast", "Image > Adjustments > Levels",
        "Image > Adjustments > Curves", "Image > Adjustments > Exposure", "Image > Adjustments > Vibrance",
        "Image > Adjustments > Hue/Saturation", "Image > Adjustments > Color Balance",
        "Image > Adjustments > Black & White", "Image > Adjustments > Photo Filter",
        "Image > Adjustments > Channel Mixer", "Image > Adjustments > Color Lookup",
        "Image > Adjustments > Invert", "Image > Adjustments > Posterize",
        "Image > Adjustments > Threshold", "Image > Adjustments > Gradient Map",
        "Image > Adjustments > Selective Color", "Image > Adjustments > Shadows/Highlights",
        "Image > Adjustments > HDR Toning", "Image > Adjustments > Desaturate",
        "Image > Adjustments > Match Color", "Image > Adjustments > Replace Color",
        "Image > Adjustments > Equalize",
        "Image > Auto Tone", "Image > Auto Contrast", "Image > Auto Color",
        "Image > Image Size", "Image > Canvas Size",
        "Image > Image Rotation > 180", "Image > Image Rotation > 90 CW",
        "Image > Image Rotation > 90 CCW", "Image > Image Rotation > Arbitrary",
        "Image > Image Rotation > Flip Horizontal", "Image > Image Rotation > Flip Vertical",
        "Image > Crop", "Image > Trim", "Image > Reveal All",
        "Image > Duplicate", "Image > Apply Image", "Image > Calculations",
        "Image > Variables > Data Sets", "Image > Analysis > Scale Marker", "Image > Analysis > Ruler Tool",

        // ---- Layer ----
        "Layer > New > Layer", "Layer > New > Layer from Background", "Layer > New > Group",
        "Layer > New > Group from Layers", "Layer > New > Artboard",
        "Layer > Duplicate Layer", "Layer > Delete > Layer", "Layer > Delete > Hidden Layers",
        "Layer > Quick Export as PNG", "Layer > Export As",
        "Layer > Layer Style > Blending Options", "Layer > Layer Style > Drop Shadow",
        "Layer > Layer Style > Inner Shadow", "Layer > Layer Style > Outer Glow",
        "Layer > Layer Style > Inner Glow", "Layer > Layer Style > Bevel and Emboss",
        "Layer > Layer Style > Satin", "Layer > Layer Style > Color Overlay",
        "Layer > Layer Style > Gradient Overlay", "Layer > Layer Style > Pattern Overlay",
        "Layer > Layer Style > Stroke", "Layer > Layer Style > Copy Layer Style",
        "Layer > Layer Style > Paste Layer Style", "Layer > Layer Style > Clear Layer Style",
        "Layer > New Fill Layer > Solid Color", "Layer > New Fill Layer > Gradient",
        "Layer > New Fill Layer > Pattern",
        "Layer > New Adjustment Layer > Levels", "Layer > New Adjustment Layer > Curves",
        "Layer > New Adjustment Layer > Hue/Saturation", "Layer > New Adjustment Layer > Color Balance",
        "Layer > New Adjustment Layer > Black & White", "Layer > New Adjustment Layer > Channel Mixer",
        "Layer > New Adjustment Layer > Gradient Map", "Layer > New Adjustment Layer > Photo Filter",
        "Layer > Layer Content Options",
        "Layer > Layer Mask > Reveal All", "Layer > Layer Mask > Hide All",
        "Layer > Layer Mask > Reveal Selection", "Layer > Layer Mask > Hide Selection",
        "Layer > Layer Mask > Delete", "Layer > Layer Mask > Apply",
        "Layer > Layer Mask > Enable", "Layer > Layer Mask > Link/Unlink",
        "Layer > Vector Mask > Reveal All", "Layer > Vector Mask > Hide All",
        "Layer > Vector Mask > Current Path", "Layer > Vector Mask > Delete",
        "Layer > Vector Mask > Enable", "Layer > Vector Mask > Link/Unlink",
        "Layer > Create Clipping Mask", "Layer > Release Clipping Mask",
        "Layer > Smart Objects > Convert to Smart Object", "Layer > Smart Objects > Rasterize",
        "Layer > Rasterize > Layer", "Layer > Rasterize > Style",
        "Layer > Rasterize > Vector Mask", "Layer > Rasterize > All Layers",
        "Layer > New Layer Based on Slice",
        "Layer > Group Layers", "Layer > Ungroup Layers", "Layer > Hide Layers",
        "Layer > Arrange > Bring to Front", "Layer > Arrange > Bring Forward",
        "Layer > Arrange > Send Backward", "Layer > Arrange > Send to Back", "Layer > Arrange > Reverse",
        "Layer > Combine Shapes > Unite", "Layer > Combine Shapes > Intersect",
        "Layer > Combine Shapes > Subtract", "Layer > Combine Shapes > Exclude",
        "Layer > Combine Shapes > Merge Shapes",
        "Layer > Align > Top Edges", "Layer > Align > Vertical Centers", "Layer > Align > Bottom Edges",
        "Layer > Align > Left Edges", "Layer > Align > Horizontal Centers", "Layer > Align > Right Edges",
        "Layer > Distribute > Top Edges", "Layer > Distribute > Vertical Centers",
        "Layer > Distribute > Bottom Edges", "Layer > Distribute > Left Edges",
        "Layer > Distribute > Horizontal Centers", "Layer > Distribute > Right Edges",
        "Layer > Lock Layers",
        "Layer > Merge Layers", "Layer > Merge Visible", "Layer > Flatten Image",
        "Layer > Matting > Defringe", "Layer > Matting > Remove Black Matte",
        "Layer > Matting > Remove White Matte",

        // ---- Select ----
        "Select > All", "Select > Deselect", "Select > Reselect", "Select > Inverse",
        "Select > All Layers", "Select > Deselect Layers", "Select > Find Layers", "Select > Isolate Layers",
        "Select > Color Range", "Select > Focus Area", "Select > Subject", "Select > Sky",
        "Select > Select and Mask",
        "Select > Modify > Border", "Select > Modify > Smooth", "Select > Modify > Expand",
        "Select > Modify > Contract", "Select > Modify > Feather",
        "Select > Grow", "Select > Similar",
        "Select > Transform Selection", "Select > Edit in Quick Mask Mode",
        "Select > Save Selection", "Select > Load Selection",

        // ---- Filter ----
        "Filter > Last Filter", "Filter > Convert for Smart Filters",
        "Filter > Filter Gallery", "Filter > Adaptive Wide Angle", "Filter > Camera Raw Filter",
        "Filter > Lens Correction", "Filter > Liquify", "Filter > Vanish Point",
        "Filter > 3D > Generate Bump Map", "Filter > 3D > Generate Normal Map",
        "Filter > Blur > Average", "Filter > Blur > Blur", "Filter > Blur > Blur More",
        "Filter > Blur > Box Blur", "Filter > Blur > Gaussian Blur", "Filter > Blur > Lens Blur",
        "Filter > Blur > Motion Blur", "Filter > Blur > Radial Blur", "Filter > Blur > Surface Blur",
        "Filter > Blur Gallery > Field Blur", "Filter > Blur Gallery > Iris Blur",
        "Filter > Blur Gallery > Tilt-Shift",
        "Filter > Distort > Displace", "Filter > Distort > Pinch", "Filter > Distort > Polar Coordinates",
        "Filter > Distort > Ripple", "Filter > Distort > Shear", "Filter > Distort > Spherize",
        "Filter > Distort > Twirl", "Filter > Distort > Wave", "Filter > Distort > ZigZag",
        "Filter > Noise > Add Noise", "Filter > Noise > Despeckle", "Filter > Noise > Dust & Scratches",
        "Filter > Noise > Median", "Filter > Noise > Reduce Noise",
        "Filter > Pixelate > Color Halftone", "Filter > Pixelate > Crystallize", "Filter > Pixelate > Facet",
        "Filter > Pixelate > Mosaic", "Filter > Pixelate > Mezzotint", "Filter > Pixelate > Pointillize",
        "Filter > Render > Clouds", "Filter > Render > Difference Clouds", "Filter > Render > Fibers",
        "Filter > Render > Lens Flare", "Filter > Render > Lighting Effects",
        "Filter > Sharpen > Sharpen", "Filter > Sharpen > Sharpen Edges", "Filter > Sharpen > Sharpen More",
        "Filter > Sharpen > Smart Sharpen", "Filter > Sharpen > Unsharp Mask",
        "Filter > Stylize > Diffuse", "Filter > Stylize > Emboss", "Filter > Stylize > Extrude",
        "Filter > Stylize > Find Edges", "Filter > Stylize > Oil Paint", "Filter > Stylize > Solarize",
        "Filter > Stylize > Tiles", "Filter > Stylize > Trace Contour", "Filter > Stylize > Wind",
        "Filter > Other > Custom", "Filter > Other > High Pass", "Filter > Other > Maximum",
        "Filter > Other > Minimum", "Filter > Other > Offset",

        // ---- View ----
        "View > Proof Setup > Custom", "View > Proof Setup > Working CMYK",
        "View > Proof Setup > Macintosh RGB", "View > Proof Setup > Internet Standard RGB",
        "View > Proof Colors", "View > Gamut Warning",
        "View > Pixel Aspect Ratio > Square", "View > Pixel Aspect Ratio > D1 VTS Aspect",
        "View > Pixel Aspect Ratio > D1 PAL", "View > Pixel Aspect Ratio > HDV 1080",
        "View > Pixel Aspect Ratio Reset",
        "View > Zoom In", "View > Zoom Out", "View > Fit on Screen", "View > 100% / Actual Pixels",
        "View > Print Size",
        "View > Screen Mode > Standard Screen Mode", "View > Screen Mode > Full Screen Mode With Menu Bar",
        "View > Screen Mode > Full Screen Mode",
        "View > Show > Target Path", "View > Show > Grid", "View > Show > Guides",
        "View > Show > Canvas Guides", "View > Show > Rulers", "View > Show > Pixel Grid",
        "View > Show > Layer Edges", "View > Show > Selection Edges",
        "View > Snap", "View > Snap To > Guides", "View > Snap To > Grid",
        "View > Snap To > Layers", "View > Snap To > Slices", "View > Snap To > Document Bounds",
        "View > Snap To > All", "View > Snap To > None",
        "View > Guides > New Guide", "View > Guides > New Guide Layout",
        "View > Guides > Lock Guides", "View > Guides > Clear Guides",

        // ---- Window ----
        "Window > Arrange > Tile All Vertically", "Window > Arrange > Tile All Horizontally",
        "Window > Arrange > Consolidate All to Tabs", "Window > Arrange > Float in Window",
        "Window > Arrange > Float All in Windows",
        "Window > Workspace > Essentials", "Window > Workspace > 3D",
        "Window > Workspace > Graphic and Web", "Window > Workspace > Motion",
        "Window > Workspace > Painting", "Window > Workspace > Photography",
        "Window > Workspace > Reset Current Workspace", "Window > Workspace > New Workspace",
        "Window > Extensions",
        "Window > 3D", "Window > Actions", "Window > Adjustments", "Window > Brush Settings",
        "Window > Brushes", "Window > Channels", "Window > Character", "Window > Color",
        "Window > History", "Window > Info", "Window > Layers", "Window > Navigator",
        "Window > Paragraph", "Window > Properties", "Window > Paths", "Window > Viewport / Scene View",
        "Window > Swatches", "Window > Timeline", "Window > Tool Settings", "Window > Tools",
        "Window > Application Frame", "Window > Options", "Window > Tools",

        // ---- Help ----
        "Help > ROSE Image Editor Help", "Help > ROSE Tutorials", "Help > Hands-on Tutorials",
        "Help > About ROSE Image Editor", "Help > System Info",
        "Help > Manage My Account", "Help > Sign Out",
    };
    static_assert(IM_ARRAYSIZE(s_CmdNames) == (size_t)Cmd::COUNT, "Cmd name table is out of sync with the Cmd enum");

    const char* CmdName(Cmd cmd)
    {
        return s_CmdNames[(int)cmd];
    }

    static void Execute(Cmd cmd)
    {
        switch (cmd)
        {
        // ---- File ----
        case Cmd::FileNew:      ShowNewDocument = true; break;
        case Cmd::FileOpen:
            if (ImageViewer::OpenFileDialog())
                SetStatus("Opened %s", ImageViewer::FileName().c_str());
            break;
        case Cmd::FileExit:     if (s_Window) glfwSetWindowShouldClose(s_Window, GLFW_TRUE); break;

        // ---- Edit ----
        case Cmd::EditPreferencesGeneral:     ShowPreferences = true; PrefTab = 0; break;
        case Cmd::EditPreferencesPerformance: ShowPreferences = true; PrefTab = 1; break;
        case Cmd::EditPreferencesDisplay:     ShowPreferences = true; PrefTab = 2; break;
        case Cmd::EditPreferencesScratchDisks:ShowPreferences = true; PrefTab = 3; break;
        case Cmd::EditPreferencesUnitsRulers: ShowPreferences = true; PrefTab = 4; break;

        // ---- View ----
        case Cmd::ViewZoomIn:       ImageViewer::ZoomIn(); break;
        case Cmd::ViewZoomOut:      ImageViewer::ZoomOut(); break;
        case Cmd::ViewFitScreen:    ImageViewer::FitScreen(); break;
        case Cmd::ViewActualPixels: ImageViewer::ActualPixels(); break;
        case Cmd::ViewShowGrid:     ShowGrid = !ShowGrid; break;
        case Cmd::ViewShowRulers:   ShowRulers = !ShowRulers; break;
        case Cmd::ViewShowGuides:   ShowGuides = !ShowGuides; break;
        case Cmd::ViewSnap:         SnapEnabled = !SnapEnabled; break;
        case Cmd::ViewGuideLockGuides: GuidesLocked = !GuidesLocked; break;

        // ---- Window panels / toggles ----
        case Cmd::WindowToggle3D:           Show3D = !Show3D; break;
        case Cmd::WindowToggleActions:      ShowActions = !ShowActions; break;
        case Cmd::WindowToggleAdjustments:  ShowAdjustments = !ShowAdjustments; break;
        case Cmd::WindowToggleBrushSettings:ShowBrushSettings = !ShowBrushSettings; break;
        case Cmd::WindowToggleBrushes:      ShowBrushes = !ShowBrushes; break;
        case Cmd::WindowToggleChannels:     ShowChannels = !ShowChannels; break;
        case Cmd::WindowToggleCharacter:    ShowCharacter = !ShowCharacter; break;
        case Cmd::WindowToggleColor:        ShowColor = !ShowColor; break;
        case Cmd::WindowToggleHistory:      ShowHistory = !ShowHistory; break;
        case Cmd::WindowToggleInfo:         ShowInfo = !ShowInfo; break;
        case Cmd::WindowToggleLayers:       ShowLayers = !ShowLayers; break;
        case Cmd::WindowToggleNavigator:    ShowNavigator = !ShowNavigator; break;
        case Cmd::WindowToggleParagraph:    ShowParagraph = !ShowParagraph; break;
        case Cmd::WindowToggleProperties:   ShowProperties = !ShowProperties; break;
        case Cmd::WindowTogglePaths:        ShowPaths = !ShowPaths; break;
        case Cmd::WindowToggleViewport:     ShowViewport = !ShowViewport; break;
        case Cmd::WindowToggleSwatches:     ShowSwatches = !ShowSwatches; break;
        case Cmd::WindowToggleTimeline:     ShowTimeline = !ShowTimeline; break;
        case Cmd::WindowToggleToolSettings: ShowToolSettings = !ShowToolSettings; break;
        case Cmd::WindowToggleTools:        ShowTools = !ShowTools; break;

        // ---- Help / Shortcuts ----
        case Cmd::EditKeyboardShortcuts: ShowKeyboardShortcuts = true; break;
        case Cmd::HelpAbout:             ShowAbout = true; break;

        default:
            // TEMPLATE STUB: every unhandled command lands here so the menu
            // structure can be exercised end-to-end before actions exist.
            SetStatus("%s: not implemented yet.", CmdName(cmd));
            break;
        }
    }

    void Dispatch()
    {
        if (s_PendingCommands.empty())
            return;
        for (Cmd cmd : s_PendingCommands)
            Execute(cmd);
        s_PendingCommands.clear();
    }

    // ---- Shortcut table -----------------------------------------------------
    // One row per binding. `showInHelp` false = hidden alternate (e.g. Ctrl+Y).
    static const ShortcutEntry s_Shortcuts[] =
    {
        // ---- File ----
        { Cmd::FileNew,             "File > New",                    "Ctrl+N",           ImGuiKey_N,        true,  false, false, true },
        { Cmd::FileOpen,            "File > Open",                   "Ctrl+O",           ImGuiKey_O,        true,  false, false, true },
        { Cmd::FileBrowseInBridge,  "File > Browse in Bridge",       "Alt+Ctrl+O",       ImGuiKey_O,        true,  false, true,  true },
        { Cmd::FileOpenAs,          "File > Open As",                "Alt+Shift+Ctrl+O", ImGuiKey_O,        true,  true,  true,  true },
        { Cmd::FileClose,           "File > Close",                  "Ctrl+W",           ImGuiKey_W,        true,  false, false, true },
        { Cmd::FileCloseAll,        "File > Close All",              "Alt+Ctrl+W",       ImGuiKey_W,        true,  false, true,  true },
        { Cmd::FileCloseAndGoToBridge, "File > Close and Go to Bridge", "Shift+Ctrl+W",  ImGuiKey_W,        true,  true,  false, true },
        { Cmd::FileSave,            "File > Save",                   "Ctrl+S",           ImGuiKey_S,        true,  false, false, true },
        { Cmd::FileSaveAs,          "File > Save As",                "Ctrl+Shift+S",     ImGuiKey_S,        true,  true,  false, true },
        { Cmd::FileSaveCopy,        "File > Save a Copy",            "Alt+Ctrl+S",       ImGuiKey_S,        true,  false, true,  true },
        { Cmd::FileRevert,          "File > Revert",                 "F12",              ImGuiKey_F12,      false, false, false, true },
        { Cmd::FileExportAs,        "File > Export As",              "Alt+Shift+Ctrl+W", ImGuiKey_W,        true,  true,  true,  true },
        { Cmd::FileSaveForWeb,      "File > Save for Web (Legacy)",  "Alt+Shift+Ctrl+S", ImGuiKey_S,        true,  true,  true,  true },
        { Cmd::FilePrint,           "File > Print",                  "Ctrl+P",           ImGuiKey_P,        true,  false, false, true },
        { Cmd::FilePrintOneCopy,    "File > Print One Copy",         "Alt+Shift+Ctrl+P", ImGuiKey_P,        true,  true,  true,  true },
        { Cmd::FileExit,            "File > Exit",                   "Alt+F4",           ImGuiKey_F4,       false, false, true,  true },

        // ---- Edit ----
        { Cmd::EditUndo,            "Edit > Undo",                   "Ctrl+Z",           ImGuiKey_Z,        true,  false, false, true },
        { Cmd::EditRedo,            "Edit > Redo",                   "Ctrl+Shift+Z",     ImGuiKey_Z,        true,  true,  false, true },
        { Cmd::EditRedo,            "Edit > Redo (alternate)",       "Ctrl+Y",           ImGuiKey_Y,        true,  false, false, false },
        { Cmd::EditToggleLastState, "Edit > Toggle Last State",      "Alt+Ctrl+Z",       ImGuiKey_Z,        true,  false, true,  true },
        { Cmd::EditFade,            "Edit > Fade",                   "Shift+Ctrl+F",     ImGuiKey_F,        true,  true,  false, true },
        { Cmd::EditCut,             "Edit > Cut",                    "Ctrl+X",           ImGuiKey_X,        true,  false, false, true },
        { Cmd::EditCopy,            "Edit > Copy",                   "Ctrl+C",           ImGuiKey_C,        true,  false, false, true },
        { Cmd::EditCopyMerged,      "Edit > Copy Merged",            "Shift+Ctrl+C",     ImGuiKey_C,        true,  true,  false, true },
        { Cmd::EditPaste,           "Edit > Paste",                  "Ctrl+V",           ImGuiKey_V,        true,  false, false, true },
        { Cmd::EditPasteInPlace,    "Edit > Paste Special > Paste in Place", "Shift+Ctrl+V", ImGuiKey_V,     true,  true,  false, true },
        { Cmd::EditPasteInto,       "Edit > Paste Special > Paste Into", "Alt+Shift+Ctrl+V", ImGuiKey_V,  true,  true,  true,  true },
        { Cmd::EditSearch,          "Edit > Search",                 "Ctrl+F",           ImGuiKey_F,        true,  false, false, true },
        { Cmd::EditFill,            "Edit > Fill",                   "Shift+F5",         ImGuiKey_F5,       false, true,  false, true },
        { Cmd::EditContentAwareScale, "Edit > Content-Aware Scale",  "Alt+Shift+Ctrl+C", ImGuiKey_C,        true,  true,  true,  true },
        { Cmd::EditFreeTransform,   "Edit > Free Transform",         "Ctrl+T",           ImGuiKey_T,        true,  false, false, true },
        { Cmd::EditTransformAgain,  "Edit > Transform > Again",      "Shift+Ctrl+T",     ImGuiKey_T,        true,  true,  false, true },
        { Cmd::EditColorSettings,   "Edit > Color Settings",         "Shift+Ctrl+K",     ImGuiKey_K,        true,  true,  false, true },
        { Cmd::EditKeyboardShortcuts, "Edit > Keyboard Shortcuts",   "Alt+Shift+Ctrl+K", ImGuiKey_K,        true,  true,  true,  true },
        { Cmd::EditMenus,           "Edit > Menus",                  "Alt+Shift+Ctrl+M", ImGuiKey_M,        true,  true,  true,  true },
        { Cmd::EditPreferencesGeneral, "Edit > Preferences > General", "Ctrl+K",         ImGuiKey_K,        true,  false, false, true },

        // ---- Image ----
        { Cmd::ImageAdjustLevels,   "Image > Adjustments > Levels",  "Ctrl+L",           ImGuiKey_L,        true,  false, false, true },
        { Cmd::ImageAdjustCurves,   "Image > Adjustments > Curves",  "Ctrl+M",           ImGuiKey_M,        true,  false, false, true },
        { Cmd::ImageAdjustHueSaturation, "Image > Adjustments > Hue/Saturation", "Ctrl+U", ImGuiKey_U,     true,  false, false, true },
        { Cmd::ImageAdjustColorBalance, "Image > Adjustments > Color Balance", "Ctrl+B", ImGuiKey_B,      true,  false, false, true },
        { Cmd::ImageAdjustBlackWhite, "Image > Adjustments > Black & White", "Alt+Shift+Ctrl+B", ImGuiKey_B, true, true, true, true },
        { Cmd::ImageAdjustInvert,   "Image > Adjustments > Invert",  "Ctrl+I",           ImGuiKey_I,        true,  false, false, true },
        { Cmd::ImageAdjustDesaturate, "Image > Adjustments > Desaturate", "Shift+Ctrl+U", ImGuiKey_U,      true,  true,  false, true },
        { Cmd::ImageAutoTone,       "Image > Auto Tone",             "Shift+Ctrl+N",     ImGuiKey_N,        true,  true,  false, true },
        { Cmd::ImageAutoContrast,   "Image > Auto Contrast",         "Alt+Shift+Ctrl+N", ImGuiKey_N,        true,  true,  true,  true },
        { Cmd::ImageAutoColor,      "Image > Auto Color",            "Shift+Ctrl+B",     ImGuiKey_B,        true,  true,  false, true },
        { Cmd::ImageSize,           "Image > Image Size",            "Ctrl+Alt+I",       ImGuiKey_I,        true,  false, true,  true },
        { Cmd::CanvasSize,          "Image > Canvas Size",           "Ctrl+Alt+C",       ImGuiKey_C,        true,  false, true,  true },

        // ---- Layer ----
        { Cmd::LayerNewLayer,       "Layer > New > Layer",           "Shift+Ctrl+N",     ImGuiKey_N,        true,  true,  false, true },
        { Cmd::LayerNewGroup,       "Layer > New > Group",           "Ctrl+G",           ImGuiKey_G,        true,  false, false, true },
        { Cmd::LayerDuplicate,      "Layer > Duplicate Layer",       "Ctrl+J",           ImGuiKey_J,        true,  false, false, true },
        { Cmd::LayerDeleteLayer,    "Layer > Delete > Layer",        "Delete",           ImGuiKey_Delete,   false, false, false, true },
        { Cmd::LayerCreateClippingMask, "Layer > Create Clipping Mask", "Alt+Ctrl+G",    ImGuiKey_G,        true,  false, true,  true },
        { Cmd::LayerReleaseClippingMask, "Layer > Release Clipping Mask", "Alt+Ctrl+G",  ImGuiKey_G,        true,  false, true,  false },
        { Cmd::LayerGroupLayers,    "Layer > Group Layers",          "Ctrl+G",           ImGuiKey_G,        true,  false, false, false },
        { Cmd::LayerUngroupLayers,  "Layer > Ungroup Layers",        "Shift+Ctrl+G",     ImGuiKey_G,        true,  true,  false, true },
        { Cmd::LayerArrangeBringToFront, "Layer > Arrange > Bring to Front", "Shift+Ctrl+]", ImGuiKey_RightBracket, true, true, false, true },
        { Cmd::LayerArrangeBringForward, "Layer > Arrange > Bring Forward", "Ctrl+]",     ImGuiKey_RightBracket, true, false, false, true },
        { Cmd::LayerArrangeSendBackward, "Layer > Arrange > Send Backward", "Ctrl+[",     ImGuiKey_LeftBracket,  true,  false, false, true },
        { Cmd::LayerArrangeSendToBack, "Layer > Arrange > Send to Back", "Shift+Ctrl+[", ImGuiKey_LeftBracket,  true,  true,  false, true },
        { Cmd::LayerMergeDown,      "Layer > Merge Layers",          "Ctrl+E",           ImGuiKey_E,        true,  false, false, true },
        { Cmd::LayerMergeVisible,   "Layer > Merge Visible",         "Shift+Ctrl+E",     ImGuiKey_E,        true,  true,  false, true },

        // ---- Select ----
        { Cmd::SelectAll,           "Select > All",                  "Ctrl+A",           ImGuiKey_A,        true,  false, false, true },
        { Cmd::SelectDeselect,      "Select > Deselect",             "Ctrl+D",           ImGuiKey_D,        true,  false, false, true },
        { Cmd::SelectReselect,      "Select > Reselect",             "Shift+Ctrl+D",     ImGuiKey_D,        true,  true,  false, true },
        { Cmd::SelectInverse,       "Select > Inverse",              "Shift+Ctrl+I",     ImGuiKey_I,        true,  true,  false, true },
        { Cmd::SelectAllLayers,     "Select > All Layers",           "Alt+Ctrl+A",       ImGuiKey_A,        true,  false, true,  true },
        { Cmd::SelectFindLayers,    "Select > Find Layers",          "Alt+Shift+Ctrl+F", ImGuiKey_F,        true,  true,  true,  true },
        { Cmd::SelectAndMask,       "Select > Select and Mask",      "Alt+Ctrl+R",       ImGuiKey_R,        true,  false, true,  true },
        { Cmd::SelectModifyFeather, "Select > Modify > Feather",     "Shift+F6",         ImGuiKey_F6,       false, true,  false, true },
        { Cmd::SelectQuickMaskMode, "Select > Edit in Quick Mask Mode", "Q",             ImGuiKey_Q,        false, false, false, true },

        // ---- Filter ----
        { Cmd::FilterLastFilter,    "Filter > Last Filter",          "Ctrl+F",           ImGuiKey_F,        true,  false, false, true },
        { Cmd::FilterLastFilter,    "Filter > Last Filter (alt)",    "Alt+Ctrl+F",       ImGuiKey_F,        true,  false, true,  false },
        { Cmd::FilterAdaptiveWideAngle, "Filter > Adaptive Wide Angle", "Alt+Shift+Ctrl+A", ImGuiKey_A,     true,  true,  true,  true },
        { Cmd::FilterCameraRaw,     "Filter > Camera Raw Filter",    "Shift+Ctrl+A",     ImGuiKey_A,        true,  true,  false, true },
        { Cmd::FilterLensCorrection,"Filter > Lens Correction",      "Shift+Ctrl+R",     ImGuiKey_R,        true,  true,  false, true },
        { Cmd::FilterLiquify,       "Filter > Liquify",              "Shift+Ctrl+X",     ImGuiKey_X,        true,  true,  false, true },
        { Cmd::FilterVanishPoint,   "Filter > Vanish Point",         "Alt+Ctrl+V",       ImGuiKey_V,        true,  false, true,  true },

        // ---- View ----
        { Cmd::ViewProofColors,     "View > Proof Colors",           "Ctrl+Y",           ImGuiKey_Y,        true,  false, false, true },
        { Cmd::ViewGamutWarning,    "View > Gamut Warning",          "Shift+Ctrl+Y",     ImGuiKey_Y,        true,  true,  false, true },
        { Cmd::ViewZoomIn,          "View > Zoom In",                "Ctrl++",           ImGuiKey_Equal,    true,  false, false, true },
        { Cmd::ViewZoomIn,          "View > Zoom In (numpad)",       "Ctrl++",           ImGuiKey_KeypadAdd, true, false, false, false },
        { Cmd::ViewZoomOut,         "View > Zoom Out",               "Ctrl+-",           ImGuiKey_Minus,    true,  false, false, true },
        { Cmd::ViewZoomOut,         "View > Zoom Out (numpad)",      "Ctrl+-",           ImGuiKey_KeypadSubtract, true, false, false, false },
        { Cmd::ViewFitScreen,       "View > Fit on Screen",          "Ctrl+0",           ImGuiKey_0,        true,  false, false, true },
        { Cmd::ViewActualPixels,    "View > 100% / Actual Pixels",   "Ctrl+1",           ImGuiKey_1,        true,  false, false, true },
        { Cmd::ViewActualPixels,    "View > 100% (alt)",             "Alt+Ctrl+0",       ImGuiKey_0,        true,  false, true,  false },
        { Cmd::ViewScreenModeFull,  "View > Screen Mode > Full Screen Mode", "F",         ImGuiKey_F,        false, false, false, true },
        { Cmd::ViewShowGrid,        "View > Show > Grid",            "Ctrl+'",           ImGuiKey_Apostrophe, true, false, false, true },
        { Cmd::ViewShowGuides,      "View > Show > Guides",          "Ctrl+;",           ImGuiKey_Semicolon, true, false, false, true },
        { Cmd::ViewShowRulers,      "View > Show > Rulers",          "Ctrl+R",           ImGuiKey_R,        true,  false, false, true },
        { Cmd::ViewShowSelectionEdges, "View > Show > Selection Edges", "Ctrl+H",        ImGuiKey_H,        true,  false, false, true },
        { Cmd::ViewSnap,            "View > Snap",                   "Shift+Ctrl+;",     ImGuiKey_Semicolon, true, true, false, true },
        { Cmd::ViewGuideLockGuides, "View > Guides > Lock Guides",   "Alt+Ctrl+;",       ImGuiKey_Semicolon, true, false, true,  true },

        // ---- Window panels ----
        { Cmd::WindowToggleActions, "Window > Actions",              "Alt+F9",           ImGuiKey_F9,       false, false, true,  true },
        { Cmd::WindowToggleBrushSettings, "Window > Brush Settings", "F5",               ImGuiKey_F5,       false, false, false, true },
        { Cmd::WindowToggleColor,   "Window > Color",                "F6",               ImGuiKey_F6,       false, false, false, true },
        { Cmd::WindowToggleInfo,    "Window > Info",                 "F8",               ImGuiKey_F8,       false, false, false, true },
        { Cmd::WindowToggleLayers,  "Window > Layers",               "F7",               ImGuiKey_F7,       false, false, false, true },

        // ---- Help ----
        { Cmd::HelpDocumentation,   "Help > ROSE Image Editor Help", "F1",               ImGuiKey_F1,       false, false, false, true },
    };

    const ShortcutEntry* GetShortcuts(int* count)
    {
        *count = (int)IM_ARRAYSIZE(s_Shortcuts);
        return s_Shortcuts;
    }

    void HandleGlobalShortcuts()
    {
        ImGuiIO& io = ImGui::GetIO();

        // Never steal text input or keys owned by an active widget.
        if (io.WantTextInput || io.WantCaptureKeyboard)
            return;
        // Skip while a menu or modal popup is open (menu items already handle it).
        if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId))
            return;

        int count = 0;
        const ShortcutEntry* shortcuts = GetShortcuts(&count);
        for (int i = 0; i < count; ++i)
        {
            const ShortcutEntry& s = shortcuts[i];
            if (io.KeyCtrl != s.ctrl)
                continue;
            if (io.KeyShift != s.shift)
                continue;
            if (io.KeyAlt != s.alt)
                continue;
            if (ImGui::IsKeyPressed(s.key, false))
            {
                Push(s.cmd);
                break; // one command per frame is enough
            }
        }
    }
}