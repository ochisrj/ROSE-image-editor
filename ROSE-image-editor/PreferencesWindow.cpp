#include "PreferencesWindow.h"
#include "imgui.h"
#include <cstdio>
#include <cfloat>

// ---- Static state ----
bool PreferencesWindow::s_openRequested = false;
bool PreferencesWindow::s_isOpen = false;
int  PreferencesWindow::s_selectedCategory = 0;
char PreferencesWindow::s_searchBuffer[128] = "";
static PreferencesState s_state;

static const char* kCategoryNames[PreferencesWindow::kCategoryCount] = {
    "General","Interface","Workspace","Tools","History & Content Credentials",
    "File Handling","Export","Performance","Image Processing","Scratch Disks",
    "Cursors","Transparency & Gamut","Units & Rulers","Guides, Grid & Slices",
    "Plugins","Type","Technology Previews"
};

void PreferencesWindow::Init() {}
void PreferencesWindow::Open(int idx){ if(idx>=0 && idx<kCategoryCount) s_selectedCategory=idx; s_openRequested=true; }
void PreferencesWindow::Close(){ s_isOpen=false; }
bool PreferencesWindow::IsOpen(){ return s_isOpen; }
PreferencesState& PreferencesWindow::State(){ return s_state; }
int  PreferencesWindow::GetSelectedCategory(){ return s_selectedCategory; }
void PreferencesWindow::SetSelectedCategory(int idx){ if(idx>=0 && idx<kCategoryCount) s_selectedCategory=idx; }

void PreferencesWindow::RenderSearchBar(){
    ImGui::SetNextItemWidth(-1);
    ImGui::InputTextWithHint("##prefs_search","Search preferences", s_searchBuffer, sizeof(s_searchBuffer));
    ImGui::Separator();
}

void PreferencesWindow::RenderLeftSidebar(){
    if(ImGui::BeginChild("PrefsSidebar", ImVec2(220, -40), true)){
        for(int i=0;i<kCategoryCount;i++){
            bool sel = (s_selectedCategory==i);
            if(ImGui::Selectable(kCategoryNames[i], sel)){
                s_selectedCategory=i;
            }
            if(sel) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndChild();
    }
}

void PreferencesWindow::RenderRightContent(){
    if(ImGui::BeginChild("PrefsContent", ImVec2(0, -40), false, ImGuiWindowFlags_AlwaysVerticalScrollbar)){
        ImGui::TextUnformatted(kCategoryNames[s_selectedCategory]);
        ImGui::Separator();
        DrawCategoryContent(s_selectedCategory);
        ImGui::EndChild();
    }
}

void PreferencesWindow::RenderBottomBar(bool* p_open){
    ImGui::Separator();
    // OK in front of Cancel per request — prevents right-aligned SetCursorPosX abort when resizing narrow
    if(ImGui::Button("OK")){
        // State is live-backed (s_state already holds widget values)
        // TODO: persist s_state to disk / apply to backend
        *p_open = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("Cancel")){
        // TODO: revert s_state to last committed copy if needed
        *p_open = false;
    }
    ImGui::SameLine();
    if(ImGui::Button("Reset Defaults")){
        s_state = PreferencesState();
    }
}

void PreferencesWindow::Render(){
    if(s_openRequested){ ImGui::OpenPopup("Preferences"); s_openRequested=false; s_isOpen=true; }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    // Prevent abort when resizing too small / close to other buttons — clamp min size
    ImGui::SetNextWindowSizeConstraints(ImVec2(720, 420), ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f,0.5f));
    ImGui::SetNextWindowSize(ImVec2(960, 620), ImGuiCond_Appearing);
    bool open = s_isOpen;
    if(!ImGui::BeginPopupModal("Preferences", &open, ImGuiWindowFlags_NoCollapse)){
        s_isOpen = open;
        return;
    }
    s_isOpen = open;
    RenderSearchBar();
    RenderLeftSidebar();
    ImGui::SameLine();
    RenderRightContent();
    RenderBottomBar(&open);
    // Single close point - BeginPopupModal with p_open handles X, buttons set open=false
    if(!open) ImGui::CloseCurrentPopup();
    s_isOpen = open;
    ImGui::EndPopup();
}

// ---- 17 panels — exact widget types per spec ----
void PreferencesWindow::DrawCategoryContent(int idx){
    auto &s = s_state;
    switch(idx){
    case 0: { // General
        ImGui::Combo("Color Picker", &s.generalColorPicker, "Adobe\0System\0");
        ImGui::Combo("HUD Color Picker", &s.generalHUDPicker, "Hue Strip (Small)\0Hue Wheel\0Hue Cube\0");
        ImGui::Combo("Image Interpolation", &s.generalInterpolation, "Bicubic Automatic\0Preserve Details 2.0\0Nearest Neighbor\0");
        ImGui::SeparatorText("Options");
        ImGui::Checkbox("Auto-Update Open File-based Documents", &s.generalAutoUpdate);
        ImGui::Checkbox("Beep When Done", &s.generalBeepWhenDone);
        ImGui::Checkbox("Auto show the Home Screen", &s.generalAutoShowHome);
        ImGui::Checkbox("Export Clipboard", &s.generalExportClipboard);
        ImGui::Checkbox("Use Legacy \"New Document\" Interface", &s.generalUseLegacyNewDoc);
        ImGui::Checkbox("Resize Image During Place", &s.generalResizeDuringPlace);
        ImGui::Checkbox("Skip Transform when Placing", &s.generalSkipTransformPlace);
        ImGui::Checkbox("Always Create Smart Objects when Placing", &s.generalSmartObjectPlace);
        ImGui::Checkbox("Use Legacy Free Transform", &s.generalLegacyFreeTransform);
        ImGui::Checkbox("Create new layer when brushing", &s.generalBrushNewLayer);
        // TODO: hook to document / placement backend
    } break;
    case 1: { // Interface
        ImGui::Text("Color Theme");
        ImGui::RadioButton("Darkest", &s.ifaceTheme, 0); ImGui::SameLine();
        ImGui::RadioButton("Dark", &s.ifaceTheme, 1); ImGui::SameLine();
        ImGui::RadioButton("Light Gray", &s.ifaceTheme, 2); ImGui::SameLine();
        ImGui::RadioButton("Light", &s.ifaceTheme, 3);
        // Spec: RadioButton OR Combo — RadioButton implemented; Combo alternative:
        // ImGui::Combo("Theme", &s.ifaceTheme, "Darkest\0Dark\0Light Gray\0Light\0");
        ImGui::ColorEdit3("Standard Screen Mode", &s.ifaceStandardColor.x);
        ImGui::ColorEdit3("Full Screen with Menus", &s.ifaceFullscreenMenuColor.x);
        ImGui::ColorEdit3("Full Screen", &s.ifaceFullscreenColor.x);
        ImGui::Combo("Border Style", &s.ifaceBorder, "Drop Shadow\0Line\0None\0");
        ImGui::Combo("UI Language", &s.ifaceLanguage, "English\0System Default\0");
        ImGui::Combo("UI Font Size", &s.ifaceFontSize, "Tiny\0Small\0Medium\0Large\0");
        ImGui::Checkbox("Scale UI to Font", &s.ifaceScaleToFont);
        ImGui::Checkbox("Show Channels in Color", &s.ifaceShowChannelsColor);
        ImGui::Checkbox("Show Tooltips", &s.ifaceShowTooltips);
        ImGui::Checkbox("Rich Tooltips", &s.ifaceRichTooltips);
    } break;
    case 2: { // Workspace
        ImGui::Checkbox("Open Documents as Tabs", &s.wsTabs);
        ImGui::Checkbox("Enable Floating Document Window Docking", &s.wsFloatingDock);
        ImGui::Checkbox("Auto-Collapse Icon Panels", &s.wsAutoCollapse);
        ImGui::Checkbox("Restore Convenient Spread", &s.wsConvenientSpread);
        ImGui::Checkbox("Large Tabs", &s.wsLargeTabs);
    } break;
    case 3: { // Tools
        ImGui::Checkbox("Show Rich Tooltips", &s.toolsRichTips);
        ImGui::Checkbox("Enable Gestures", &s.toolsGestures);
        ImGui::Checkbox("Use Shift Key for Tool Switch", &s.toolsShiftSwitch);
        ImGui::Checkbox("Over scroll", &s.toolsOverscroll);
        ImGui::Checkbox("Enable Animated Zoom", &s.toolsAnimatedZoom);
        ImGui::Checkbox("Zoom with Scroll Wheel", &s.toolsScrollZoom);
        ImGui::Checkbox("Zoom Clicked Point to Center", &s.toolsZoomCenter);
        ImGui::Checkbox("Enable Scrubby Zoom", &s.toolsScrubbyZoom);
        ImGui::Checkbox("Arrow Keys Rotate Brush", &s.toolsArrowRotateBrush);
    } break;
    case 4: { // History & Content Credentials
        ImGui::SliderInt("History States", &s.histStates, 1, 1000);
        // Alternative per spec: ImGui::InputInt("History States", &s.histStates);
        ImGui::Combo("History Log Destination", &s.histLogDestination, "Metadata\0Text File\0Both\0");
        ImGui::Checkbox("Content Credentials (Beta)", &s.histContentCredentials);
        // TODO: history log backend
    } break;
    case 5: { // File Handling
        ImGui::Combo("Image Previews", &s.fhPreview, "Always Save\0Never Save\0Ask When Saving\0");
        ImGui::Text("File Extension");
        ImGui::RadioButton("Lowercase", &s.fhExtension, 0); ImGui::SameLine();
        ImGui::RadioButton("Uppercase", &s.fhExtension, 1);
        ImGui::Checkbox("Save As to Original Folder", &s.fhOriginalFolder);
        ImGui::Checkbox("Save in Background", &s.fhBackgroundSave);
        ImGui::Checkbox("Enable Cloud Document Saver", &s.fhCloudSaver);
        ImGui::Combo("Auto-Save Interval", &s.fhAutoSave, "5 Minutes\010 Minutes\015 Minutes\030 Minutes\01 Hour\0");
        ImGui::InputInt("Max Recent Files", &s.fhMaxRecent);
        if(ImGui::Button("Camera Raw Preferences...")){ /* TODO: open Camera Raw dialog */ }
    } break;
    case 6: { // Export
        ImGui::Combo("Quick Export Format", &s.expFormat, "PNG\0JPG\0GIF\0SVG\0");
        ImGui::Text("Export Location");
        ImGui::RadioButton("Ask every time", &s.expLocation, 0); ImGui::SameLine();
        ImGui::RadioButton("Export to relative folder", &s.expLocation, 1);
        ImGui::Combo("Export Metadata", &s.expMetadata, "Copyright & Contact Info\0None\0");
        ImGui::Checkbox("Convert to sRGB", &s.expToSRGB);
    } break;
    case 7: { // Performance
        ImGui::SliderInt("RAM Allocation (%)", &s.perfRamPercent, 5, 100, "%d%%");
        ImGui::Checkbox("Use Graphics Processor (GPU)", &s.perfUseGPU);
        ImGui::SameLine();
        if(ImGui::Button("Advanced Settings...")){ ImGui::OpenPopup("GPUAdvanced"); /* TODO */ }
        ImGui::Combo("Cache Tile Size", &s.perfTileSize, "128K\0256K\0512K\01024K\0");
        ImGui::SliderInt("Cache Levels", &s.perfCacheLevels, 1, 8);
    } break;
    case 8: { // Image Processing
        ImGui::Combo("Remove Background Engine", &s.ipRemoveBg, "Cloud (More accurate)\0Device (Faster)\0");
        ImGui::Combo("Select Subject Engine", &s.ipSelectSubject, "Cloud (Detailed results)\0Device (Faster results)\0");
        ImGui::Combo("Color Conversion", &s.ipColorConversion, "Managed\0Unmanaged\0");
    } break;
    case 9: { // Scratch Disks
        if(ImGui::BeginTable("ScratchDisks", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)){
            ImGui::TableSetupColumn("Drive"); ImGui::TableSetupColumn("Status");
            ImGui::TableHeadersRow();
            for(auto &d : s.scratchDrives){
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Checkbox(d.path.c_str(), &d.active);
                ImGui::TableNextColumn();
                ImGui::Text("%s", d.status.c_str());
            }
            ImGui::EndTable();
        }
        // TODO: enumerate system drives
    } break;
    case 10: { // Cursors
        ImGui::Text("Painting Cursors");
        ImGui::RadioButton("Standard", &s.curPainting, 0); ImGui::SameLine();
        ImGui::RadioButton("Precise", &s.curPainting, 1); ImGui::SameLine();
        ImGui::RadioButton("Normal Brush Tip", &s.curPainting, 2); ImGui::SameLine();
        ImGui::RadioButton("Full Size Brush Tip", &s.curPainting, 3);
        ImGui::Checkbox("Show Crosshair in Brush Tip", &s.curCrosshairTip);
        ImGui::Checkbox("Show Only Crosshair While Painting", &s.curOnlyCrosshair);
        ImGui::SeparatorText("Other Cursors");
        ImGui::RadioButton("Standard##other", &s.curOther, 0); ImGui::SameLine();
        ImGui::RadioButton("Precise##other", &s.curOther, 1);
        ImGui::ColorEdit4("Brush Preview Color", &s.curPreviewColor.x);
    } break;
    case 11: { // Transparency & Gamut
        ImGui::Combo("Grid Size", &s.tgGridSize, "None\0Small\0Medium\0Large\0");
        ImGui::Combo("Grid Colors", &s.tgGridColors, "Light\0Medium\0Dark\0Red\0Custom\0");
        ImGui::ColorEdit3("Gamut Warning Color", &s.tgGamutColor.x);
        ImGui::SliderFloat("Gamut Opacity", &s.tgGamutOpacity, 0.0f, 100.0f, "%.0f%%");
    } break;
    case 12: { // Units & Rulers
        ImGui::Combo("Rulers", &s.urRuler, "Pixels\0Inches\0Centimeters\0Millimeters\0Points\0Picas\0Percent\0");
        ImGui::Combo("Type", &s.urType, "Pixels\0Inches\0Centimeters\0Millimeters\0Points\0Picas\0Percent\0");
        ImGui::InputFloat("Column Width", &s.urColumnWidth);
        ImGui::InputFloat("Gutter", &s.urGutter);
        ImGui::InputFloat("Print Resolution", &s.urResPrint);
        ImGui::InputFloat("Screen Resolution", &s.urResScreen);
        ImGui::Text("Point/Pica Size");
        ImGui::RadioButton("PostScript (72 pt/in)", &s.urPointPica, 0); ImGui::SameLine();
        ImGui::RadioButton("Traditional (72.27 pt/in)", &s.urPointPica, 1);
    } break;
    case 13: { // Guides, Grid & Slices
        ImGui::ColorEdit3("Guides", &s.ggsGuideColor.x);
        ImGui::ColorEdit3("Smart Guides", &s.ggsSmartGuideColor.x);
        ImGui::ColorEdit3("Grid", &s.ggsGridColor.x);
        ImGui::ColorEdit3("Slices", &s.ggsSliceColor.x);
        ImGui::ColorEdit3("Count Tool", &s.ggsCountColor.x);
        ImGui::InputFloat("Grid Every", &s.ggsGridEvery);
        ImGui::InputInt("Subdivisions", &s.ggsSubdiv);
    } break;
    case 14: { // Plugins
        ImGui::Checkbox("Enable Additional Plugins Folder", &s.plugAdditionalFolder);
        ImGui::Checkbox("Allow Extensions to Connect to the Internet", &s.plugAllowInternet);
        ImGui::Checkbox("Load Panel Generator", &s.plugGenerator);
        ImGui::Checkbox("Legacy Extensions", &s.plugLegacy);
        if(ImGui::Button("Choose Folder...")){ /* TODO: directory picker */ }
    } break;
    case 15: { // Type
        ImGui::Checkbox("Use Smart Quotes", &s.typeSmartQuotes);
        ImGui::Checkbox("Enable Japanese Crop Marks", &s.typeJapaneseCrop);
        ImGui::Checkbox("Show Font Names in English", &s.typeEnglishNames);
        ImGui::InputInt("Recent Fonts Count", &s.typeRecentFonts);
        ImGui::Text("Text Engine");
        ImGui::RadioButton("East Asian", &s.typeEngine, 0); ImGui::SameLine();
        ImGui::RadioButton("World-Ready Layout", &s.typeEngine, 1);
    } break;
    case 16: { // Technology Previews
        ImGui::Checkbox("Enable Native Canvas", &s.tpNativeCanvas);
        ImGui::Checkbox("Enable Precise Zooming", &s.tpPreciseZoom);
        ImGui::Checkbox("Enable Content-Aware Tracing Tool", &s.tpContentAwareTrace);
        ImGui::Checkbox("Multithreaded Compositing", &s.tpMultiThreadComp);
        ImGui::Checkbox("Deactivate Native Canvas", &s.tpDeactivateNative);
    } break;
    default: break;
    }
}
 