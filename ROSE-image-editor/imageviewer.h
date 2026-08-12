#pragma once
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

class ImageViewer
{
public:
    static void DrawWindow();
    static bool IsVisible();
    static void SetVisible(bool visible);
    static void Shutdown();

private:
    struct PendingImage
    {
        std::string path;
        int width = 0;
        int height = 0;
        int channels = 0;
        double decodeMs = 0.0;
        std::vector<unsigned char> pixels;
        bool failed = false;
        std::string error;
    };

    static bool PickFile(std::string& outPath);
    static void RequestLoad(const std::string& path);
    static void PollLoadResult();
    static void ClearTexture();
    static std::string WideToUtf8(const std::wstring& w);
    static std::string FormatBytes(uint64_t bytes);

    static bool          s_Visible;
    static bool          s_Loading;
    static unsigned int  s_Texture;
    static int           s_Width;
    static int           s_Height;
    static int           s_Channels;
    static float         s_Zoom;
    static float         s_PanX;
    static float         s_PanY;
    static bool          s_Dragging;
    static double        s_DecodeMs;
    static double        s_UploadMs;

    static std::string   s_FileName;
    static std::string   s_Location;
    static std::string   s_Resolution;
    static std::string   s_AspectRatio;
    static std::string   s_FileSize;
    static std::string   s_Error;

    static std::atomic<bool> s_LoadInProgress;
    static std::mutex        s_ResultMutex;
    static bool              s_ResultReady;
    static PendingImage      s_Result;
};
