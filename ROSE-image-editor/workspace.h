#pragma once
#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

struct ImDrawList;
struct ImVec2;

// The Workspace is the central document area. It owns every loaded image
// (texture + camera state) and renders the active one inside a pannable,
// zoomable canvas that supports rulers, an alignment grid and glowing guides.
//
// The Workspace exposes no internal properties panel and no window-level menu
// bar; it relies on the main menu bar for all controls.
class Workspace
{
public:
    static void DrawWindow();
    static bool IsVisible();
    static void SetVisible(bool visible);
    static void Shutdown();

    static bool OpenFileDialog();
    static void LoadFile(const std::string& path);

    static void ZoomIn();
    static void ZoomOut();
    static void FitScreen();
    static void ActualPixels();
    static void ResetView();

    static bool HasImage();
    static const std::string& FileName();
    static float GetZoom();
    static int   GetImageWidth();
    static int   GetImageHeight();

    // Opens the "Add Guide" modal (View > Guides > Add Guide... / command).
    static void OpenAddGuideDialog();

    // Clears every guide on the active document (View > Guides > Clear Guides
    // / canvas context menu).
    static void ClearAllGuides();

    // Closes the active tab (tab X button / File > Close / Ctrl+W), freeing its
    // OpenGL texture and switching to the adjacent tab.
    static void CloseActive();

    // Closes every open image (File > Close All / Alt+Ctrl+W).
    static void CloseAllImages();

private:
    struct Guide
    {
        bool  horizontal = false;
        float position   = 0.0f;   // in image pixels
    };

    struct Image
    {
        unsigned int texture = 0;
        int width = 0;
        int height = 0;
        int channels = 0;
        std::string filePath;
        std::string fileName;      // file name + extension, used as the tab title
        std::string location;
        std::string resolution;
        std::string aspectRatio;
        std::string fileSize;
        float  zoom = 1.0f;
        float  panX = 0.0f;
        float  panY = 0.0f;
        std::vector<Guide> guides;
    };

    struct PendingImage
    {
        std::string path;
        int width = 0;
        int height = 0;
        int channels = 0;
        std::vector<unsigned char> pixels;
        bool failed = false;
        std::string error;
    };

    static bool PickFile(std::string& outPath);
    static void RequestLoadNext();
    static void PollLoadResult();
    static void ClearTextures();
    static std::string WideToUtf8(const std::wstring& w);
    static std::string FormatBytes(uint64_t bytes);

    static void DrawTabBar();
    static void DrawCanvas();
    static void DrawGrid(ImDrawList* dl, const ImVec2& imageOrigin, float zoom,
                         const ImVec2& canvasMin, const ImVec2& canvasMax);
    static void DrawRulers(ImDrawList* dl, const ImVec2& imageOrigin, float zoom,
                           const ImVec2& canvasMin, const ImVec2& canvasMax, float rulerSize);
    static void DrawGuides(ImDrawList* dl, const std::vector<Guide>& guides,
                           const ImVec2& imageOrigin, float zoom,
                           const ImVec2& canvasMin, const ImVec2& canvasMax, int highlightIndex);
    static void DrawGuideDialog();

    static void CloseImage(int index);

    static Image* ActiveImage();

    // Pure logic helpers, kept separate from the draw path.
    static int   SelectTab(int current, int clicked, int count);
    static float ClampGuidePosition(float position, float maxPosition);
    static int   HitTestGuide(const Image& img, const ImVec2& imageOrigin, float zoom,
                              const ImVec2& mousePos, float threshold);

    static bool           s_Visible;
    static bool           s_Loading;
    static int            s_ActiveTab;
    static float          s_CanvasW;
    static float          s_CanvasH;
    static bool           s_Dragging;
    static bool           s_PanLockX;
    static bool           s_PanLockY;
    static int            s_GuideDrag;       // -1 = none, else index into active image guides
    static int            s_RulerDrag;       // -1 = none, 0 = top ruler (horizontal guide), 1 = left ruler (vertical guide)
    static bool           s_OpenGuideDialog;
    static bool           s_ForceTabSelect;

    static std::string    s_Error;

    static std::vector<Image>   s_Images;
    static std::vector<std::string> s_LoadQueue;

    static std::atomic<bool> s_LoadInProgress;
    static std::mutex        s_ResultMutex;
    static bool              s_ResultReady;
    static PendingImage      s_Result;
};
