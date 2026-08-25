#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

#include <glad/glad.h>

#include "imgui.h"
#include "imgui_internal.h"

#include <stb/stb_image.h>

#include "workspace.h"

#include "appstate.h"
#include "rulerscale.h"

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

bool           Workspace::s_Visible = false;
bool           Workspace::s_Loading = false;
int            Workspace::s_ActiveTab = 0;
float          Workspace::s_CanvasW = 0.0f;
float          Workspace::s_CanvasH = 0.0f;
bool           Workspace::s_Dragging = false;
bool           Workspace::s_PanLockX = false;
bool           Workspace::s_PanLockY = false;
int            Workspace::s_GuideDrag = -1;
int            Workspace::s_RulerDrag = -1;
bool           Workspace::s_OpenGuideDialog = false;
bool           Workspace::s_ForceTabSelect = false;

std::string    Workspace::s_Error;

std::vector<Workspace::Image>         Workspace::s_Images;
std::vector<std::string>              Workspace::s_LoadQueue;

std::atomic<bool> Workspace::s_LoadInProgress = false;
std::mutex        Workspace::s_ResultMutex;
bool              Workspace::s_ResultReady = false;
Workspace::PendingImage Workspace::s_Result;

std::string Workspace::WideToUtf8(const std::wstring& w)
{
    if (w.empty())
        return "";
    const int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string out(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), &out[0], len, nullptr, nullptr);
    return out;
}

std::string Workspace::FormatBytes(uint64_t bytes)
{
    const double kb = 1024.0;
    const double mb = 1024.0 * 1024.0;
    char buf[64];
    if (bytes >= mb)
        snprintf(buf, sizeof(buf), "%.2f MB", bytes / mb);
    else if (bytes >= kb)
        snprintf(buf, sizeof(buf), "%.2f KB", bytes / kb);
    else
        snprintf(buf, sizeof(buf), "%llu B", (unsigned long long)bytes);
    return buf;
}

bool Workspace::PickFile(std::string& outPath)
{
    wchar_t buffer[MAX_PATH * 2] = { 0 };
    OPENFILENAMEW ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = (HWND)ImGui::GetMainViewport()->PlatformHandleRaw;
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.tga;*.webp;*.psd;*.ico\0All Files\0*.*\0";
    ofn.lpstrFile = buffer;
    ofn.nMaxFile = MAX_PATH * 2;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    ofn.lpstrTitle = L"Open Image";

    if (GetOpenFileNameW(&ofn))
    {
        outPath = WideToUtf8(buffer);
        return true;
    }
    return false;
}

void Workspace::ClearTextures()
{
    for (Image& img : s_Images)
    {
        if (img.texture != 0)
        {
            glDeleteTextures(1, &img.texture);
            img.texture = 0;
        }
    }
    s_Images.clear();
    s_ActiveTab = 0;
    s_GuideDrag = -1;
}

// Start decoding the next queued path on a worker thread. Loads are serialized:
// only one decode runs at a time and PollLoadResult() pulls the result back on
// the main thread, where GL texture creation must happen.
void Workspace::RequestLoadNext()
{
    bool expected = false;
    if (!s_LoadInProgress.compare_exchange_strong(expected, true))
        return;
    if (s_LoadQueue.empty())
    {
        s_LoadInProgress = false;
        return;
    }

    std::string path = s_LoadQueue.front();
    s_LoadQueue.erase(s_LoadQueue.begin());

    s_Loading = true;
    s_Error.clear();
    s_ResultReady = false;

    std::thread worker([path]() {
        PendingImage result;
        result.path = path;

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
        {
            result.failed = true;
            result.error = "Failed to open file.";
        }
        else
        {
            std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)),
                                              std::istreambuf_iterator<char>());
            file.close();

            if (buffer.empty())
            {
                result.failed = true;
                result.error = "File is empty.";
            }
            else
            {
                int w = 0, h = 0, comp = 0;
                unsigned char* data = stbi_load_from_memory(buffer.data(), (int)buffer.size(),
                                                            &w, &h, &comp, STBI_rgb_alpha);
                if (!data)
                {
                    result.failed = true;
                    result.error = std::string("Unsupported or corrupt image: ") +
                                   (stbi_failure_reason() ? stbi_failure_reason() : "unknown error");
                }
                else
                {
                    result.width = w;
                    result.height = h;
                    result.channels = comp;
                    const size_t count = (size_t)w * h * 4;
                    result.pixels.assign(data, data + count);
                    stbi_image_free(data);
                }
            }
        }

        {
            std::lock_guard<std::mutex> lock(s_ResultMutex);
            s_Result = std::move(result);
            s_ResultReady = true;
        }
    });
    worker.detach();
}

void Workspace::PollLoadResult()
{
    PendingImage result;
    bool ready = false;
    {
        std::lock_guard<std::mutex> lock(s_ResultMutex);
        if (s_ResultReady)
        {
            result = std::move(s_Result);
            s_ResultReady = false;
            ready = true;
        }
    }
    if (!ready)
        return;

    s_LoadInProgress = false;
    s_Loading = false;

    if (result.failed)
    {
        s_Error = result.error;
        RequestLoadNext();
        return;
    }

    Image img;
    img.filePath = result.path;
    img.width = result.width;
    img.height = result.height;
    img.channels = result.channels;

    glGenTextures(1, &img.texture);
    glBindTexture(GL_TEXTURE_2D, img.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, result.width, result.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, result.pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    fs::path p(result.path);
    img.fileName = p.filename().string();
    img.location = p.parent_path().string();
    if (img.location.empty())
        img.location = p.root_path().string();
    img.resolution = std::to_string(img.width) + " * " + std::to_string(img.height) + " px";
    img.aspectRatio = std::to_string((double)img.width / (double)img.height);

    std::error_code ec;
    const uintmax_t fsize = fs::file_size(p, ec);
    img.fileSize = FormatBytes(ec ? 0 : fsize);

    s_Images.push_back(std::move(img));
    s_ActiveTab = (int)s_Images.size() - 1;

    RequestLoadNext();
}

bool Workspace::IsVisible()
{
    return s_Visible;
}

void Workspace::SetVisible(bool visible)
{
    s_Visible = visible;
}

void Workspace::Shutdown()
{
    ClearTextures();
    s_Visible = false;
}

bool Workspace::OpenFileDialog()
{
    std::string path;
    if (!PickFile(path))
        return false;
    LoadFile(path);
    return true;
}

void Workspace::LoadFile(const std::string& path)
{
    if (path.empty())
        return;
    App::AddRecent(path);
    s_Visible = true;
    s_LoadQueue.push_back(path);
    RequestLoadNext();
}

Workspace::Image* Workspace::ActiveImage()
{
    if (s_Images.empty() || s_ActiveTab < 0 || s_ActiveTab >= (int)s_Images.size())
        return nullptr;
    return &s_Images[s_ActiveTab];
}

void Workspace::ZoomIn()
{
    if (Image* img = ActiveImage())
        img->zoom = fminf(32.0f, fmaxf(0.02f, img->zoom * 1.25f));
}

void Workspace::ZoomOut()
{
    if (Image* img = ActiveImage())
        img->zoom = fminf(32.0f, fmaxf(0.02f, img->zoom / 1.25f));
}

void Workspace::FitScreen()
{
    Image* img = ActiveImage();
    if (!img || s_CanvasW <= 0.0f || s_CanvasH <= 0.0f)
        return;
    const float scaleX = s_CanvasW / (float)img->width;
    const float scaleY = s_CanvasH / (float)img->height;
    img->zoom = fminf(32.0f, fmaxf(0.02f, fminf(scaleX, scaleY)));
    img->panX = 0.0f;
    img->panY = 0.0f;
}

void Workspace::ActualPixels()
{
    if (Image* img = ActiveImage())
        img->zoom = 1.0f;
}

void Workspace::ResetView()
{
    if (Image* img = ActiveImage())
    {
        img->zoom = 1.0f;
        img->panX = 0.0f;
        img->panY = 0.0f;
    }
}

bool Workspace::HasImage()
{
    Image* img = ActiveImage();
    return img && img->texture != 0 && img->width > 0 && img->height > 0;
}

const std::string& Workspace::FileName()
{
    static const std::string empty;
    Image* img = ActiveImage();
    return img ? img->fileName : empty;
}

float Workspace::GetZoom()
{
    Image* img = ActiveImage();
    return img ? img->zoom : 1.0f;
}

int Workspace::GetImageWidth()
{
    Image* img = ActiveImage();
    return img ? img->width : 0;
}

int Workspace::GetImageHeight()
{
    Image* img = ActiveImage();
    return img ? img->height : 0;
}

void Workspace::OpenAddGuideDialog()
{
    s_OpenGuideDialog = true;
}

void Workspace::ClearAllGuides()
{
    if (Image* img = ActiveImage())
    {
        img->guides.clear();
        s_GuideDrag = -1;
    }
}

void Workspace::CloseImage(int index)
{
    if (index < 0 || index >= (int)s_Images.size())
        return;

    if (s_Images[index].texture != 0)
    {
        glDeleteTextures(1, &s_Images[index].texture);
        s_Images[index].texture = 0;
    }

    const int closed = index;
    s_Images.erase(s_Images.begin() + index);
    s_GuideDrag = -1;
    s_RulerDrag = -1;

    if (s_Images.empty())
    {
        s_ActiveTab = 0;
        return;
    }

    const int last = (int)s_Images.size() - 1;
    if (closed < s_ActiveTab)
        --s_ActiveTab;                             // image before the closed one stays active
    else if (closed == s_ActiveTab && s_ActiveTab > last)
        s_ActiveTab = last;                        // closed the last tab, fall back to previous
    if (s_ActiveTab < 0) s_ActiveTab = 0;
    if (s_ActiveTab > last) s_ActiveTab = last;
}

void Workspace::CloseActive()
{
    if (!s_Images.empty())
        CloseImage(s_ActiveTab);
}

void Workspace::CloseAllImages()
{
    for (Image& img : s_Images)
    {
        if (img.texture != 0)
        {
            glDeleteTextures(1, &img.texture);
            img.texture = 0;
        }
    }
    s_Images.clear();
    s_ActiveTab = 0;
    s_GuideDrag = -1;
    s_RulerDrag = -1;
}

// ---------------------------------------------------------------------------
// Pure, unit-testable logic
// ---------------------------------------------------------------------------

int Workspace::SelectTab(int current, int clicked, int count)
{
    if (clicked < 0 || clicked >= count)
        return current;
    return clicked;
}

float Workspace::ClampGuidePosition(float position, float maxPosition)
{
    return fminf(fmaxf(0.0f, position), fmaxf(0.0f, maxPosition));
}

int Workspace::HitTestGuide(const Image& img, const ImVec2& imageOrigin, float zoom,
                            const ImVec2& mousePos, float threshold)
{
    for (int i = 0; i < (int)img.guides.size(); ++i)
    {
        const Guide& g = img.guides[i];
        if (g.horizontal)
        {
            const float y = imageOrigin.y + g.position * zoom;
            if (fabsf(mousePos.y - y) <= threshold)
                return i;
        }
        else
        {
            const float x = imageOrigin.x + g.position * zoom;
            if (fabsf(mousePos.x - x) <= threshold)
                return i;
        }
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Canvas drawing
// ---------------------------------------------------------------------------

void Workspace::DrawGrid(ImDrawList* dl, const ImVec2& imageOrigin, float zoom,
                         const ImVec2& canvasMin, const ImVec2& canvasMax)
{
    const RulerScale scale = ComputeRulerScale(zoom);
    const float step = (float)scale.minor;
    const ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(0.45f, 0.45f, 0.50f, 0.22f));

    const int kMinX = (int)ceilf((canvasMin.x - imageOrigin.x) / zoom / step);
    const int kMaxX = (int)floorf((canvasMax.x - imageOrigin.x) / zoom / step);
    for (int k = kMinX; k <= kMaxX; ++k)
    {
        const float x = imageOrigin.x + (float)k * step * zoom;
        dl->AddLine(ImVec2(x, canvasMin.y), ImVec2(x, canvasMax.y), col, 1.0f);
    }

    const int kMinY = (int)ceilf((canvasMin.y - imageOrigin.y) / zoom / step);
    const int kMaxY = (int)floorf((canvasMax.y - imageOrigin.y) / zoom / step);
    for (int k = kMinY; k <= kMaxY; ++k)
    {
        const float y = imageOrigin.y + (float)k * step * zoom;
        dl->AddLine(ImVec2(canvasMin.x, y), ImVec2(canvasMax.x, y), col, 1.0f);
    }
}

static void TickLabel(int value, char* buf, size_t len)
{
    snprintf(buf, len, "%d", value);
}

void Workspace::DrawRulers(ImDrawList* dl, const ImVec2& imageOrigin, float zoom,
                           const ImVec2& canvasMin, const ImVec2& canvasMax, float rulerSize)
{
    const RulerScale scale = ComputeRulerScale(zoom);
    const float majorStep = (float)scale.major;
    const float minorStep = (float)scale.minor;
    const float majorLen = 9.0f;
    const float minorLen = 3.5f;

    const ImU32 bgCol   = ImGui::ColorConvertFloat4ToU32(ImVec4(0.16f, 0.16f, 0.17f, 0.92f));
    const ImU32 edgeCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.35f, 0.35f, 0.38f, 1.0f));
    const ImU32 tickCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.72f, 0.72f, 0.75f, 0.9f));
    const ImU32 textCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.78f, 0.78f, 0.82f, 1.0f));

    // Backgrounds (drawn as two strips; the left one wins over the corner).
    dl->AddRectFilled(ImVec2(canvasMin.x, canvasMin.y), ImVec2(canvasMax.x, canvasMin.y + rulerSize), bgCol);
    dl->AddRectFilled(ImVec2(canvasMin.x, canvasMin.y), ImVec2(canvasMin.x + rulerSize, canvasMax.y), bgCol);

    char buf[32];

    // --- Top ruler (horizontal pixel positions) ---
    const float rx0 = canvasMin.x + rulerSize;              // skip the corner

    // Minor ticks, unlabeled, drawn first so the major ticks can overlay them.
    const int mMinX = (int)ceilf((rx0 - imageOrigin.x) / zoom / minorStep);
    const int mMaxX = (int)floorf((canvasMax.x - imageOrigin.x) / zoom / minorStep);
    for (int m = mMinX; m <= mMaxX; ++m)
    {
        const float x = imageOrigin.x + (float)m * minorStep * zoom;
        dl->AddLine(ImVec2(x, canvasMin.y), ImVec2(x, canvasMin.y + minorLen), tickCol, 1.0f);
    }

    // Major ticks, labeled. The RulerScale guarantees two consecutive major
    // ticks are >= kLabelGapPx screen pixels apart, so labels never overlap.
    const int MMinX = (int)ceilf((rx0 - imageOrigin.x) / zoom / majorStep);
    const int MMaxX = (int)floorf((canvasMax.x - imageOrigin.x) / zoom / majorStep);
    for (int M = MMinX; M <= MMaxX; ++M)
    {
        const float x = imageOrigin.x + (float)M * majorStep * zoom;
        TickLabel((int)llroundf((float)M * majorStep), buf, sizeof(buf));
        const ImVec2 textSize = ImGui::CalcTextSize(buf);
        if (x + 2.0f + textSize.x > canvasMax.x)
            continue;
        dl->AddLine(ImVec2(x, canvasMin.y), ImVec2(x, canvasMin.y + majorLen), tickCol, 1.0f);
        dl->AddText(ImVec2(x + 2.0f, canvasMin.y + 2.0f), textCol, buf);
    }
    dl->AddLine(ImVec2(canvasMin.x, canvasMin.y + rulerSize), ImVec2(canvasMax.x, canvasMin.y + rulerSize), edgeCol, 1.0f);

    // --- Left ruler (vertical pixel positions) ---
    const float ry0 = canvasMin.y + rulerSize;              // skip the corner

    const int mMinY = (int)ceilf((ry0 - imageOrigin.y) / zoom / minorStep);
    const int mMaxY = (int)floorf((canvasMax.y - imageOrigin.y) / zoom / minorStep);
    for (int m = mMinY; m <= mMaxY; ++m)
    {
        const float y = imageOrigin.y + (float)m * minorStep * zoom;
        dl->AddLine(ImVec2(canvasMin.x, y), ImVec2(canvasMin.x + minorLen, y), tickCol, 1.0f);
    }

    const int MMinY = (int)ceilf((ry0 - imageOrigin.y) / zoom / majorStep);
    const int MMaxY = (int)floorf((canvasMax.y - imageOrigin.y) / zoom / majorStep);
    for (int M = MMinY; M <= MMaxY; ++M)
    {
        const float y = imageOrigin.y + (float)M * majorStep * zoom;
        TickLabel((int)llroundf((float)M * majorStep), buf, sizeof(buf));
        const ImVec2 textSize = ImGui::CalcTextSize(buf);
        if (y + 2.0f + textSize.x > canvasMax.y)
            continue;
        dl->AddLine(ImVec2(canvasMin.x, y), ImVec2(canvasMin.x + majorLen, y), tickCol, 1.0f);
        dl->AddText(ImVec2(canvasMin.x + 3.0f, y + 2.0f), textCol, buf);
    }
    dl->AddLine(ImVec2(canvasMin.x + rulerSize, canvasMin.y), ImVec2(canvasMin.x + rulerSize, canvasMax.y), edgeCol, 1.0f);
}

void Workspace::DrawGuides(ImDrawList* dl, const std::vector<Guide>& guides,
                           const ImVec2& imageOrigin, float zoom,
                           const ImVec2& canvasMin, const ImVec2& canvasMax, int highlightIndex)
{
    // Thin, glowing cyan guides as specified; the highlighted (hovered/dragged)
    // guide gets a wider glow and a bright white core for visibility.
    const ImU32 glowCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.9f, 1.0f, 0.30f));
    const ImU32 lineCol = ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.9f, 1.0f, 1.0f));
    const ImU32 hiCol   = ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

    for (int i = 0; i < (int)guides.size(); ++i)
    {
        const Guide& g = guides[i];
        const bool highlighted = (i == highlightIndex);
        const float glowW = highlighted ? 7.0f : 4.0f;
        const ImU32 core  = highlighted ? hiCol : lineCol;

        if (g.horizontal)
        {
            const float y = imageOrigin.y + g.position * zoom;
            dl->AddLine(ImVec2(canvasMin.x, y), ImVec2(canvasMax.x, y), glowCol, glowW);
            dl->AddLine(ImVec2(canvasMin.x, y), ImVec2(canvasMax.x, y), core, 1.0f);
        }
        else
        {
            const float x = imageOrigin.x + g.position * zoom;
            dl->AddLine(ImVec2(x, canvasMin.y), ImVec2(x, canvasMax.y), glowCol, glowW);
            dl->AddLine(ImVec2(x, canvasMin.y), ImVec2(x, canvasMax.y), core, 1.0f);
        }
    }
}

void Workspace::DrawCanvas()
{
    const ImVec2 canvasMin = ImGui::GetWindowPos();
    const ImVec2 canvasMax = ImVec2(canvasMin.x + ImGui::GetWindowWidth(),
                                    canvasMin.y + ImGui::GetWindowHeight());

    Image* img = ActiveImage();
    ImGuiIO& io = ImGui::GetIO();

    // Rulers (when enabled) inset the image region along the top and left edges.
    const float rulerSize = (App::ShowRulers && img) ? 18.0f : 0.0f;
    const ImVec2 workMin = ImVec2(canvasMin.x + rulerSize, canvasMin.y + rulerSize);
    const float centerX = (workMin.x + canvasMax.x) * 0.5f;
    const float centerY = (workMin.y + canvasMax.y) * 0.5f;

    if (!img)
    {
        if (s_Loading)
        {
            ImGui::SetCursorPos(ImVec2(12.0f, 12.0f));
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Loading image...");
        }
        else
        {
            const float textW = 220.0f;
            ImGui::SetCursorPos(ImVec2((s_CanvasW - textW) * 0.5f, (s_CanvasH - 40.0f) * 0.5f));
            ImGui::TextDisabled("No image loaded");
            ImGui::SameLine();
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
            if (ImGui::Button("Open Image..."))
            {
                std::string path;
                if (PickFile(path))
                    LoadFile(path);
            }
        }
        return;
    }

    // ---- Geometry. preOrigin is the image top-left for this frame's input;
    //      guide hit-testing uses it so a drag is never offset by panning. ----
    float dispW = (float)img->width * img->zoom; 
    float dispH = (float)img->height * img->zoom;  
    float halfW = dispW * 0.5f;                   
    float halfH = dispH * 0.5f;
    const float canvasHalfW = (canvasMax.x - workMin.x) * 0.5f;
    const float canvasHalfH = (canvasMax.y - workMin.y) * 0.5f;


    const ImVec2 preOrigin(centerX - halfW + img->panX, centerY - halfH + img->panY);

    const bool hovered = ImGui::IsWindowHovered();
    const bool ctrl    = io.KeyCtrl;
    const bool shift   = io.KeyShift;
    const bool midDown = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
    const bool lmbDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    const bool guidesInteractive = App::ShowGuides && !img->guides.empty();

    // ---- 0) Drag-from-ruler guide creation (Photoshop style) ----
    // Clicking the top ruler spawns a horizontal guide; clicking the left
    // ruler spawns a vertical one. While held, a preview follows the cursor;
    // releasing over the canvas commits it, releasing over the ruler cancels.
    if (App::ShowRulers && App::ShowGuides && hovered && s_RulerDrag < 0 &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        const ImVec2 mp = io.MousePos;
        if (mp.y >= canvasMin.y && mp.y < workMin.y && mp.x >= workMin.x)
        {
            s_RulerDrag = 0;   // top ruler  -> horizontal guide
            s_Dragging = false;
            s_GuideDrag = -1;
        }
        else if (mp.x >= canvasMin.x && mp.x < workMin.x && mp.y >= workMin.y)
        {
            s_RulerDrag = 1;   // left ruler -> vertical guide
            s_Dragging = false;
            s_GuideDrag = -1;
        }
    }

    if (s_RulerDrag >= 0)
    {
        const bool horizontal = (s_RulerDrag == 0);
        const float maxPos = horizontal ? (float)img->height : (float)img->width;
        ImGui::SetMouseCursor(horizontal ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_ResizeEW);
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            const bool overRuler = horizontal
                ? (io.MousePos.y < workMin.y)
                : (io.MousePos.x < workMin.x);
            if (!overRuler)
            {
                Guide g;
                g.horizontal = horizontal;
                g.position = ClampGuidePosition(
                    horizontal ? (io.MousePos.y - preOrigin.y) / img->zoom
                               : (io.MousePos.x - preOrigin.x) / img->zoom,
                    maxPos);
                img->guides.push_back(g);
                App::ShowGuides = true; 
            }
            s_RulerDrag = -1;
        }
    }

    // ---- 1) Left click: dragging a guide wins over panning ----
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left) && s_RulerDrag < 0)
    {
        const int hit = guidesInteractive
            ? HitTestGuide(*img, preOrigin, img->zoom, io.MousePos, 6.0f)
            : -1;
        if (hit >= 0)
        {
            s_GuideDrag = hit;
            s_Dragging = false;
            s_PanLockX = s_PanLockY = false;
        }
        else
        {
            s_GuideDrag = -1;
            s_Dragging = true;
            s_PanLockX = s_PanLockY = false;
        }
    }
    if (!lmbDown)
        s_GuideDrag = -1;

    // ---- 2) Middle mouse: axis-locked panning ----
    // Ctrl + MMB: X-axis only. Shift + MMB: Y-axis only. Plain MMB: free.
    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Middle))
    {
        s_Dragging = true;
        s_PanLockX = s_PanLockY = false;
        if (ctrl)
            s_PanLockY = true;
        if (shift)
            s_PanLockX = true;
        s_GuideDrag = -1;
    }
    if (!midDown && !lmbDown)
        s_Dragging = false;

    // ---- 3) Apply the active interaction ----
    if (s_GuideDrag >= 0 && s_GuideDrag < (int)img->guides.size())
    {
        // Live-guide drag: map the cursor back into image pixel space.
        Guide& g = img->guides[s_GuideDrag];
        const float val = g.horizontal
            ? (io.MousePos.y - preOrigin.y) / img->zoom
            : (io.MousePos.x - preOrigin.x) / img->zoom;

        // Display position value during drag
        ImGui::SetTooltip("%s: %.0f px", g.horizontal ? "Y" : "X", val);

        g.position = ClampGuidePosition(val, g.horizontal ? (float)img->height : (float)img->width);
        ImGui::SetMouseCursor(g.horizontal ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_ResizeEW);
    }
    else if (s_Dragging)
    {
        img->panX += s_PanLockY ? 0.0f : io.MouseDelta.x;
        img->panY += s_PanLockX ? 0.0f : io.MouseDelta.y;
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    //else
    //{
    //    // No guide dragging or panning: set cursor based on active tool
    //    // Only set if we are not creating a guide from the ruler
    //    if (s_RulerDrag < 0)
    //    {
    //        switch (ToolPanel::GetActiveTool())
    //        {
    //            case ToolPanel::ToolType::Move:
    //                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    //                break;
    //            case ToolPanel::ToolType::Marquee:
    //                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    //                break;
    //            case ToolPanel::ToolType::Lasso:
    //                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    //                break;
    //            case ToolPanel::ToolType::Eyedropper:
    //                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    //                break;
    //            case ToolPanel::ToolType::Hand:
    //                ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
    //                break;
    //            case ToolPanel::ToolType::Zoom:
    //                ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    //                break;
    //        }
    //    }
    //}

    // ---- 4) Mouse-wheel zoom, anchored at the cursor ----
    // An exponential mapping on the accumulated wheel delta (io.MouseWheel is
    // a float that can exceed 1.0 on fast scrolls) makes the zoom proportional
    // to the actual scroll amount and independent of frame rate, eliminating
    // the stutter caused by a fixed per-frame factor.
    if (hovered && io.MouseWheel != 0.0f && s_GuideDrag < 0)
    {
        constexpr float kZoomSpeed = 0.08f;   // ~8% per wheel notch
        constexpr float kMinZoom = 0.02f;
        constexpr float kMaxZoom = 32.0f;

        const float newZoom = fminf(kMaxZoom, fmaxf(kMinZoom,
            img->zoom * expf(io.MouseWheel * kZoomSpeed)));

        if (newZoom != img->zoom)
        {
            const float rel = newZoom / img->zoom;
            img->panX = (io.MousePos.x - centerX) * (1.0f - rel) + img->panX * rel;
            img->panY = (io.MousePos.y - centerY) * (1.0f - rel) + img->panY * rel;
            img->zoom = newZoom;

            // zoom เปลี่ยนแล้ว ค่า disp/half ที่ cache ไว้ตอนต้นฟังก์ชัน "เก่า" ไปแล้ว
            // ต้องคำนวณใหม่ ไม่งั้น clamp (step 5) กับตัวภาพที่วาดจริง (step 6)
            // จะใช้ขนาดของเฟรมก่อนหน้า ทำให้ภาพหลุดตำแหน่งไป 1 เฟรมทุกครั้งที่ scroll
            // — ยิ่ง zoom สูงยิ่งเห็นเป็นอาการสั่นชัด เพราะ dispW/H แปรผันตาม zoom โดยตรง
            dispW = (float)img->width * img->zoom;
            dispH = (float)img->height * img->zoom;
            halfW = dispW * 0.5f;
            halfH = dispH * 0.5f;
        }
    }

    // ---- 5) Clamp pan so the image can't be lost entirely off-screen. A
    // floor is applied so zooming across the "image smaller than canvas"
    // boundary never snaps the pan back to centre - that snap fought the
    // cursor-anchored zoom and produced the visible jitter/bounce.
    const float maxPanX = fmaxf(fmaxf(0.0f, halfW - canvasHalfW), canvasHalfW * 0.25f);
    const float maxPanY = fmaxf(fmaxf(0.0f, halfH - canvasHalfH), canvasHalfH * 0.25f);
    img->panX = fminf(maxPanX, fmaxf(-maxPanX, img->panX));
    img->panY = fminf(maxPanY, fmaxf(-maxPanY, img->panY));

    //const ImVec2 imageOrigin(centerX - halfW + img->panX, centerY - halfH + img->panY);
    //const ImVec2 p1(imageOrigin.x + dispW, imageOrigin.y + dispH);

    ImVec2 imageOrigin(centerX - halfW + img->panX, centerY - halfH + img->panY);
    imageOrigin.x = roundf(imageOrigin.x);
    imageOrigin.y = roundf(imageOrigin.y);
    const ImVec2 p1(imageOrigin.x + roundf(dispW), imageOrigin.y + roundf(dispH));

    ImDrawList* dl = ImGui::GetWindowDrawList();

    // Background alignment grid (behind the image).
    if (App::ShowGrid)
        DrawGrid(dl, imageOrigin, img->zoom, workMin, canvasMax);

    // The active image's texture is looked up fresh every frame from the
    // selected tab, so switching tabs immediately changes the bound texture.
    dl->AddImage((ImTextureID)(intptr_t)img->texture, imageOrigin, p1);

    // Glowing cyan guides on top of the image.
    if (App::ShowGuides)
    {
        int highlight = s_GuideDrag;
        if (highlight < 0)
            highlight = HitTestGuide(*img, imageOrigin, img->zoom, io.MousePos, 6.0f);
        if (highlight >= 0)
        {
            const Guide& hg = img->guides[highlight];
            ImGui::SetMouseCursor(hg.horizontal ? ImGuiMouseCursor_ResizeNS : ImGuiMouseCursor_ResizeEW);
        }
        DrawGuides(dl, img->guides, imageOrigin, img->zoom, workMin, canvasMax, highlight);

        // Live preview while pulling a guide out of a ruler.
        if (s_RulerDrag >= 0)
        {
            Guide preview;
            preview.horizontal = (s_RulerDrag == 0);
            const float maxPos = preview.horizontal ? (float)img->height : (float)img->width;
            preview.position = ClampGuidePosition(
                preview.horizontal ? (io.MousePos.y - imageOrigin.y) / img->zoom
                                   : (io.MousePos.x - imageOrigin.x) / img->zoom,
                maxPos);
            std::vector<Guide> previews = img->guides;
            previews.push_back(preview);
            DrawGuides(dl, previews, imageOrigin, img->zoom, workMin, canvasMax,
                       (int)previews.size() - 1);
        }
    }

    // Rulers along the canvas edges.
    if (App::ShowRulers)
        DrawRulers(dl, imageOrigin, img->zoom, canvasMin, canvasMax, rulerSize);

    if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ImGui::OpenPopup("##workspace_context");

    if (ImGui::BeginPopup("##workspace_context"))
    {
        if (ImGui::MenuItem("Add Guide..."))
            OpenAddGuideDialog();
        ImGui::Separator();
        if (ImGui::MenuItem("Clear All Guides", nullptr, false, !img->guides.empty()))
        {
            img->guides.clear();
            s_GuideDrag = -1;
        }
        ImGui::EndPopup();
    }
}
// ในไฟล์ header เพิ่ม static member:
static bool s_ForceTabSelect;

// workspace.cpp

void Workspace::DrawTabBar()
{
    if (s_Images.size() <= 1)
        return;

    int closeIndex = -1;

    if (ImGui::BeginTabBar("##workspace_tabs", ImGuiTabBarFlags_None))
    {
        for (int i = 0; i < (int)s_Images.size(); ++i)
        {
            Image& img = s_Images[i];
            bool open = true;
            ImGui::PushID(i);

            ImGuiTabItemFlags flags = (s_ForceTabSelect && i == s_ActiveTab)
                ? ImGuiTabItemFlags_SetSelected : 0;

            if (ImGui::BeginTabItem(img.fileName.c_str(), &open, flags))
            {
                s_ActiveTab = i;   // <-- ImGui บอกตรงๆ ว่าแท็บนี้ active ตอนนี้
                ImGui::EndTabItem();
            }
            ImGui::PopID();

            if (!open)
                closeIndex = i;
        }
        ImGui::EndTabBar();
    }

    s_ForceTabSelect = false;

    if (closeIndex >= 0)
        CloseImage(closeIndex);
}

void Workspace::DrawGuideDialog()
{
    if (s_OpenGuideDialog)
    {
        ImGui::OpenPopup("Add Guide");
        s_OpenGuideDialog = false;
    }

    if (!ImGui::BeginPopupModal("Add Guide", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        return;

    static int   orientation = 0;   // 0 = Horizontal, 1 = Vertical
    static float position = 100.0f;

    const char* orientationNames[] = { "Horizontal", "Vertical" };
    ImGui::SetNextItemWidth(180.0f);
    ImGui::Combo("Orientation", &orientation, orientationNames, 2);

    ImGui::SetNextItemWidth(180.0f);
    ImGui::InputFloat("Position (px)", &position, 1.0f, 10.0f, "%.1f");

    ImGui::Separator();

    ImGui::BeginDisabled(!HasImage());
    if (ImGui::Button("Add", ImVec2(90.0f, 0.0f)))
    {
        if (Image* img = ActiveImage())
        {
            const bool horizontal = orientation == 0;
            const float maxPos = horizontal ? (float)img->height : (float)img->width;
            Guide g;
            g.horizontal = horizontal;
            g.position = ClampGuidePosition(position, maxPos);
            img->guides.push_back(g);
            App::ShowGuides = true;  
        }
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndDisabled();

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(90.0f, 0.0f)))
        ImGui::CloseCurrentPopup();

    ImGui::EndPopup();
}

void Workspace::DrawWindow()
{
    PollLoadResult();

    if (!s_Visible)
        return;

    // No menu bar, no custom styling overrides: the Workspace uses the default
    // ImGui look and is driven entirely from the main menu bar.
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Workspace", &s_Visible))
    {
        ImGui::End();
        return;
    }

    DrawTabBar();

    const ImVec2 region = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize(region.x, region.y);
    if (canvasSize.x < 1.0f) canvasSize.x = 1.0f;
    if (canvasSize.y < 1.0f) canvasSize.y = 1.0f;
    s_CanvasW = canvasSize.x;
    s_CanvasH = canvasSize.y;

    ImGui::BeginChild("##canvas", canvasSize, ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
    DrawCanvas();
    ImGui::EndChild();

    DrawGuideDialog();

    ImGui::End();
}
