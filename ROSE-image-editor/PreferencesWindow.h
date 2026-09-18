#pragma once
// PreferencesWindow.h — Structural skeleton for Photoshop-style Preferences
// 17 categories, sidebar navigation (BeginChild + Selectable), right panel rendering.
// No backend logic — all widgets backed by primitive members in PreferencesState.

#include <string>
#include <vector>
#include "imgui.h"

struct PreferencesState {
    // 1. General
    int  generalColorPicker = 0;      // Combo: Adobe, System
    int  generalHUDPicker = 0;        // Combo: Hue Strip (Small), Hue Wheel, Hue Cube
    int  generalInterpolation = 0;    // Combo: Bicubic Automatic, Preserve Details 2.0, Nearest Neighbor
    bool generalAutoUpdate = true;    // Checkbox
    bool generalBeepWhenDone = false;
    bool generalAutoShowHome = true;
    bool generalExportClipboard = true;
    bool generalUseLegacyNewDoc = false;
    bool generalResizeDuringPlace = true;
    bool generalSkipTransformPlace = false;
    bool generalSmartObjectPlace = true;
    bool generalLegacyFreeTransform = false;
    bool generalBrushNewLayer = false;

    // 2. Interface
    int    ifaceTheme = 1;            // RadioButton: Darkest, Dark, Light Gray, Light (or Combo)
    ImVec4 ifaceStandardColor = ImVec4(0.12f,0.12f,0.12f,1.0f); // ColorEdit3
    ImVec4 ifaceFullscreenMenuColor = ImVec4(0.08f,0.08f,0.08f,1.0f);
    ImVec4 ifaceFullscreenColor = ImVec4(0.05f,0.05f,0.05f,1.0f);
    int    ifaceBorder = 0;           // Combo: Drop Shadow, Line, None
    int    ifaceLanguage = 0;         // Combo
    int    ifaceFontSize = 2;         // Combo: Tiny, Small, Medium, Large
    bool   ifaceScaleToFont = true;
    bool   ifaceShowChannelsColor = false;
    bool   ifaceShowTooltips = true;
    bool   ifaceRichTooltips = true;

    // 3. Workspace
    bool wsTabs = true;
    bool wsFloatingDock = true;
    bool wsAutoCollapse = true;
    bool wsConvenientSpread = false;
    bool wsLargeTabs = false;

    // 4. Tools
    bool toolsRichTips = true;
    bool toolsGestures = true;
    bool toolsShiftSwitch = true;
    bool toolsOverscroll = true;
    bool toolsAnimatedZoom = true;
    bool toolsScrollZoom = false;
    bool toolsZoomCenter = true;
    bool toolsScrubbyZoom = true;
    bool toolsArrowRotateBrush = false;

    // 5. History & Content Credentials
    int  histStates = 50;             // InputInt / SliderInt 1-1000
    int  histLogDestination = 0;      // Combo: Metadata, Text File, Both
    bool histContentCredentials = false; // Checkbox (Beta)

    // 6. File Handling
    int  fhPreview = 0;               // Combo: Always Save, Never Save, Ask When Saving
    int  fhExtension = 0;             // RadioButton: Lowercase, Uppercase
    bool fhOriginalFolder = false;
    bool fhBackgroundSave = true;
    bool fhCloudSaver = false;
    int  fhAutoSave = 1;              // Combo: 5/10/15/30 Minutes, 1 Hour
    int  fhMaxRecent = 20;            // InputInt

    // 7. Export
    int  expFormat = 0;               // Combo: PNG, JPG, GIF, SVG
    int  expLocation = 0;             // RadioButton: Ask every time, Export to relative folder
    int  expMetadata = 0;             // Combo: Copyright & Contact Info, None
    bool expToSRGB = true;            // Checkbox: Convert to sRGB

    // 8. Performance
    int  perfRamPercent = 70;         // SliderInt % RAM
    bool perfUseGPU = true;          // Checkbox
    int  perfTileSize = 1;            // Combo: 128K, 256K, 1024K...
    int  perfCacheLevels = 4;         // SliderInt 1-8

    // 9. Image Processing
    int  ipRemoveBg = 0;              // Combo: Cloud (More accurate), Device (Faster)
    int  ipSelectSubject = 0;         // Combo: Cloud (Detailed), Device (Faster)
    int  ipColorConversion = 0;       // Combo

    // 10. Scratch Disks
    struct Drive { std::string path = "C:\\"; bool active = true; std::string status = "120 GB free"; };
    std::vector<Drive> scratchDrives{ {"C:\\",true,"120 GB free"}, {"D:\\",false,"340 GB free"} };

    // 11. Cursors
    int    curPainting = 2;           // RadioButton: Standard, Precise, Normal Brush Tip, Full Size Brush Tip
    bool   curCrosshairTip = false;
    bool   curOnlyCrosshair = false;
    int    curOther = 0;              // RadioButton: Standard, Precise
    ImVec4 curPreviewColor = ImVec4(1,0,0,0.8f); // ColorEdit4

    // 12. Transparency & Gamut
    int    tgGridSize = 2;            // Combo: None, Small, Medium, Large
    int    tgGridColors = 1;          // Combo: Light, Medium, Dark, Red, Custom
    ImVec4 tgGamutColor = ImVec4(1,0,1,1); // ColorEdit3
    float  tgGamutOpacity = 100.0f;   // SliderFloat 0-100

    // 13. Units & Rulers
    int   urRuler = 0;                // Combo: Pixels, Inches, Centimeters... Percent
    int   urType = 0;
    float urColumnWidth = 180.0f;     // InputFloat
    float urGutter = 12.0f;
    float urResPrint = 300.0f;
    float urResScreen = 72.0f;
    int   urPointPica = 0;            // RadioButton: PostScript (72 pt/in), Traditional (72.27)

    // 14. Guides, Grid & Slices
    ImVec4 ggsGuideColor = ImVec4(0,1,1,1);       // ColorEdit3/4
    ImVec4 ggsSmartGuideColor = ImVec4(1,0,1,1);
    ImVec4 ggsGridColor = ImVec4(0.5f,0.5f,0.5f,1);
    ImVec4 ggsSliceColor = ImVec4(0,1,0,1);
    ImVec4 ggsCountColor = ImVec4(1,0.5f,0,1);
    float  ggsGridEvery = 100.0f;     // InputFloat
    int    ggsSubdiv = 4;             // InputInt

    // 15. Plugins
    bool plugAdditionalFolder = false;
    bool plugAllowInternet = true;
    bool plugGenerator = true;
    bool plugLegacy = false;

    // 16. Type
    bool typeSmartQuotes = true;
    bool typeJapaneseCrop = false;
    bool typeEnglishNames = true;
    int  typeRecentFonts = 10;        // InputInt
    int  typeEngine = 1;              // RadioButton: East Asian, World-Ready Layout

    // 17. Technology Previews
    bool tpNativeCanvas = false;
    bool tpPreciseZoom = false;
    bool tpContentAwareTrace = false;
    bool tpMultiThreadComp = true;
    bool tpDeactivateNative = false;
};

class PreferencesWindow {
public:
    static constexpr int kCategoryCount = 17;
    static void Init();
    static void Open(int categoryIndex = 0);
    static void Close();
    static bool IsOpen();
    static void Render();
    static PreferencesState& State();
    static int  GetSelectedCategory();
    static void SetSelectedCategory(int idx);
private:
    static void RenderSearchBar();
    static void RenderLeftSidebar();
    static void RenderRightContent();
    static void RenderBottomBar(bool* p_open);
    static void DrawCategoryContent(int idx);
    static bool s_openRequested;
    static bool s_isOpen;
    static int  s_selectedCategory;
    static char s_searchBuffer[128];
};
