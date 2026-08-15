#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>

#include <glad/glad.h>

#include "imgui.h"

#include <stb/stb_image.h>

#include "imageviewer.h"

#include "appstate.h"

#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <thread>

namespace fs = std::filesystem;

bool          ImageViewer::s_Visible = false;
bool          ImageViewer::s_Loading = false;
unsigned int  ImageViewer::s_Texture = 0;
int           ImageViewer::s_Width = 0;
int           ImageViewer::s_Height = 0;
int           ImageViewer::s_Channels = 0;
float         ImageViewer::s_Zoom = 1.0f;
float         ImageViewer::s_PanX = 0.0f;
float         ImageViewer::s_PanY = 0.0f;
float         ImageViewer::s_CanvasW = 0.0f;
float         ImageViewer::s_CanvasH = 0.0f;
bool          ImageViewer::s_Dragging = false;
double        ImageViewer::s_DecodeMs = 0.0;
double        ImageViewer::s_UploadMs = 0.0;

std::string   ImageViewer::s_FileName;
std::string   ImageViewer::s_Location;
std::string   ImageViewer::s_Resolution;
std::string   ImageViewer::s_AspectRatio;
std::string   ImageViewer::s_FileSize;
std::string   ImageViewer::s_Error;

std::atomic<bool> ImageViewer::s_LoadInProgress = false;
std::mutex        ImageViewer::s_ResultMutex;
bool              ImageViewer::s_ResultReady = false;
ImageViewer::PendingImage ImageViewer::s_Result;

std::string ImageViewer::WideToUtf8(const std::wstring& w)
{
    if (w.empty())
        return "";
    const int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string out(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), &out[0], len, nullptr, nullptr);
    return out;
}

std::string ImageViewer::FormatBytes(uint64_t bytes)
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

bool ImageViewer::PickFile(std::string& outPath)
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

void ImageViewer::ClearTexture()
{
    if (s_Texture != 0)
    {
        glDeleteTextures(1, &s_Texture);
        s_Texture = 0;
    }
}

void ImageViewer::RequestLoad(const std::string& path)
{
    bool expected = false;
    if (!s_LoadInProgress.compare_exchange_strong(expected, true))
        return;

    s_Loading = true;
    s_Error.clear();
    s_ResultReady = false;

    std::thread worker([path]() {
        PendingImage result;
        result.path = path;

        const auto t0 = std::chrono::steady_clock::now();

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

        result.decodeMs = std::chrono::duration<double, std::milli>(
            std::chrono::steady_clock::now() - t0).count();

        {
            std::lock_guard<std::mutex> lock(s_ResultMutex);
            s_Result = std::move(result);
            s_ResultReady = true;
        }
    });
    worker.detach();
}

void ImageViewer::PollLoadResult()
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
        return;
    }

    ClearTexture();

    const auto t0 = std::chrono::steady_clock::now();

    glGenTextures(1, &s_Texture);
    glBindTexture(GL_TEXTURE_2D, s_Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, result.width, result.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, result.pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    s_UploadMs = std::chrono::duration<double, std::milli>(
        std::chrono::steady_clock::now() - t0).count();

    s_Width = result.width;
    s_Height = result.height;
    s_Channels = result.channels;
    s_DecodeMs = result.decodeMs;
    s_Zoom = 1.0f;
    s_PanX = 0.0f;
    s_PanY = 0.0f;

    fs::path p(result.path);
    s_FileName = p.filename().string();
    s_Location = p.parent_path().string();
    if (s_Location.empty())
        s_Location = p.root_path().string();
    s_Resolution = std::to_string(s_Width) + " * " + std::to_string(s_Height) + " px";
    s_AspectRatio = std::to_string((double)s_Width / (double)s_Height);

    std::error_code ec;
    const uintmax_t fsize = fs::file_size(p, ec);
    s_FileSize = FormatBytes(ec ? 0 : fsize);
}

bool ImageViewer::IsVisible()
{
    return s_Visible;
}

void ImageViewer::SetVisible(bool visible)
{
    s_Visible = visible;
}

void ImageViewer::Shutdown()
{
    ClearTexture();
    s_Visible = false;
}

bool ImageViewer::OpenFileDialog()
{
    std::string path;
    if (!PickFile(path))
        return false;
    LoadFile(path);
    return true;
}

void ImageViewer::LoadFile(const std::string& path)
{
    if (path.empty())
        return;
    App::AddRecent(path);
    RequestLoad(path);
    s_Visible = true;
}

void ImageViewer::ZoomIn()
{
    s_Zoom = fminf(32.0f, fmaxf(0.02f, s_Zoom * 1.25f));
}

void ImageViewer::ZoomOut()
{
    s_Zoom = fminf(32.0f, fmaxf(0.02f, s_Zoom / 1.25f));
}

void ImageViewer::FitScreen()
{
    if (!HasImage() || s_CanvasW <= 0.0f || s_CanvasH <= 0.0f)
        return;
    const float scaleX = s_CanvasW / (float)s_Width;
    const float scaleY = s_CanvasH / (float)s_Height;
    s_Zoom = fminf(32.0f, fmaxf(0.02f, fminf(scaleX, scaleY)));
    s_PanX = 0.0f;
    s_PanY = 0.0f;
}

void ImageViewer::ActualPixels()
{
    s_Zoom = 1.0f;
}

void ImageViewer::ResetView()
{
    s_Zoom = 1.0f;
    s_PanX = 0.0f;
    s_PanY = 0.0f;
}

bool ImageViewer::HasImage()
{
    return s_Texture != 0 && s_Width > 0 && s_Height > 0;
}

const std::string& ImageViewer::FileName()
{
    return s_FileName;
}

float ImageViewer::GetZoom()
{
    return s_Zoom;
}

int ImageViewer::GetImageWidth()
{
    return s_Width;
}

int ImageViewer::GetImageHeight()
{
    return s_Height;
}

void ImageViewer::DrawWindow()
{
    PollLoadResult();

    if (!s_Visible)
        return;

    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Image Viewer", &s_Visible, ImGuiWindowFlags_MenuBar))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            std::string path;
            if (ImGui::MenuItem("Open", "Ctrl+O"))
            {
                if (PickFile(path))
                    RequestLoad(path);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close", NULL, false, s_Visible))
                s_Visible = false;
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::TextDisabled("Zoom");
            if (ImGui::MenuItem("Zoom In",  "+")) s_Zoom = fminf(32.0f, fmaxf(0.02f, s_Zoom * 1.25f));
            if (ImGui::MenuItem("Zoom Out", "-")) s_Zoom = fminf(32.0f, fmaxf(0.02f, s_Zoom / 1.25f));
            if (ImGui::MenuItem("Reset View", "1:1"))
            {
                s_Zoom = 1.0f;
                s_PanX = 0.0f;
                s_PanY = 0.0f;
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    const float infoHeight = 180.0f;
    const ImVec2 region = ImGui::GetContentRegionAvail();
    ImVec2 canvasSize(region.x, region.y - infoHeight);
    if (canvasSize.x < 1.0f) canvasSize.x = 1.0f;
    if (canvasSize.y < 1.0f) canvasSize.y = 1.0f;
    s_CanvasW = canvasSize.x;
    s_CanvasH = canvasSize.y;

    // --- Canvas ---
    ImGui::BeginChild("##canvas", canvasSize, ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);

    const ImVec2 canvasMin = ImGui::GetWindowPos();
    const ImVec2 canvasMax = ImVec2(canvasMin.x + ImGui::GetWindowWidth(),
                                    canvasMin.y + ImGui::GetWindowHeight());
    const float centerX = (canvasMin.x + canvasMax.x) * 0.5f;
    const float centerY = (canvasMin.y + canvasMax.y) * 0.5f;

    ImGuiIO& io = ImGui::GetIO();

    if (s_Texture != 0 && s_Width > 0 && s_Height > 0)
    {
        // --- RMB drag-pan state machine ---
        const bool hovered = ImGui::IsWindowHovered();
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            s_Dragging = true;
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
            s_Dragging = false;

        if (s_Dragging)
        {
            s_PanX += io.MouseDelta.x;
            s_PanY += io.MouseDelta.y;
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        }

        // --- Mouse-wheel zoom, anchored at the cursor ---
        if (hovered && io.MouseWheel != 0.0f)
        {
            const float factor = io.MouseWheel > 0.0f ? 1.1f : 0.9f;
            const float newZoom = fminf(32.0f, fmaxf(0.02f, s_Zoom * factor));
            const float rel = newZoom / s_Zoom;
            const float ax = io.MousePos.x - centerX;
            const float ay = io.MousePos.y - centerY;
            s_PanX = ax * (1.0f - rel) + s_PanX * rel;
            s_PanY = ay * (1.0f - rel) + s_PanY * rel;
            s_Zoom = newZoom;
        }

        // --- Clamp pan so the image can't get lost entirely off-screen ---
        const float dispW = (float)s_Width * s_Zoom;
        const float dispH = (float)s_Height * s_Zoom;
        const float halfW = dispW * 0.5f;
        const float halfH = dispH * 0.5f;
        const float canvasHalfW = (canvasMax.x - canvasMin.x) * 0.5f;
        const float canvasHalfH = (canvasMax.y - canvasMin.y) * 0.5f;
        const float maxPanX = fmaxf(0.0f, halfW - canvasHalfW);
        const float maxPanY = fmaxf(0.0f, halfH - canvasHalfH);
        s_PanX = fminf(maxPanX, fmaxf(-maxPanX, s_PanX));
        s_PanY = fminf(maxPanY, fmaxf(-maxPanY, s_PanY));

        // --- Draw image centered, offset by pan ---
        const ImVec2 p0(centerX - halfW + s_PanX, centerY - halfH + s_PanY);
        const ImVec2 p1(p0.x + dispW, p0.y + dispH);

        ImGui::GetWindowDrawList()->AddImage((ImTextureID)(intptr_t)s_Texture, p0, p1);

        // --- Overlay HUD ---
        ImGui::SetCursorPos(ImVec2(6.0f, 4.0f));
        ImGui::Text("Zoom: %.1f%%", s_Zoom * 100.0f);
        ImGui::SameLine();
        ImGui::TextDisabled("(wheel: zoom, RMB drag: pan)");
        ImGui::SameLine();
        if (ImGui::SmallButton("Reset View"))
        {
            s_Zoom = 1.0f;
            s_PanX = 0.0f;
            s_PanY = 0.0f;
        }
    }
    else if (s_Loading)
    {
        ImGui::SetCursorPos(ImVec2(12.0f, 12.0f));
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "Loading image...");
    }
    else
    {
        const float textW = 220.0f;
        ImGui::SetCursorPos(ImVec2((canvasSize.x - textW) * 0.5f, (canvasSize.y - 40.0f) * 0.5f));
        ImGui::TextDisabled("No image loaded");
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.0f);
        if (ImGui::Button("File > Open"))
        {
            std::string path;
            if (PickFile(path))
                RequestLoad(path);
        }
    }

    ImGui::EndChild();

    // --- Properties Panel ---
    ImGui::Separator();
    ImGui::Text("Image Properties");

    const float panelH = infoHeight - ImGui::GetFrameHeightWithSpacing();

    ImGui::BeginChild("##props", ImVec2(ImGui::GetContentRegionAvail().x, panelH), false, ImGuiWindowFlags_NoMove);

    if (s_Texture == 0)
    {
        if (!s_Error.empty())
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Error: %s", s_Error.c_str());
        else
            ImGui::TextDisabled("Open an image to view its properties.");
    }
    else
    {
        const char* channelsStr = (s_Channels == 4) ? "RGBA" :
                                  (s_Channels == 3) ? "RGB"  :
                                  (s_Channels == 2) ? "Gray + Alpha" :
                                  (s_Channels == 1) ? "Grayscale" : "Unknown";

        const uint64_t vram = (uint64_t)s_Width * (uint64_t)s_Height * 4;

        if (ImGui::BeginTable("props", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            const auto row = [](const char* label, const char* fmt, ...) {
                va_list args;
                va_start(args, fmt);
                char buf[512];
                vsnprintf(buf, sizeof(buf), fmt, args);
                va_end(args);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(label);
                ImGui::TableSetColumnIndex(1);
                ImGui::TextWrapped("%s", buf);
            };

            row("File Name", "%s", s_FileName.c_str());
            row("File Path", "%s", s_Location.c_str());
            row("Dimensions", "%s", s_Resolution.c_str());
            row("Aspect Ratio", "%s", s_AspectRatio.c_str());
            row("Channels", "%s", channelsStr);
            row("File Size", "%s", s_FileSize.c_str());
            row("GL Texture ID", "%u", s_Texture);
            row("VRAM (approx)", "%s", FormatBytes(vram).c_str());
            row("Decode Time", "%.1f ms", s_DecodeMs);
            row("Upload Time", "%.1f ms", s_UploadMs);
            row("Zoom Level", "%.1f %%", s_Zoom * 100.0f);
            row("Pan Offset", "X: %.0f  Y: %.0f", s_PanX, s_PanY);

            ImGui::EndTable();
        }
    }

    ImGui::EndChild();
    ImGui::End();
}