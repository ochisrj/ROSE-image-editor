// ============================================================================
//  PHOTOSHOP CLONE  —  Single-File C++ / OpenGL / ImGui
//  Built on top of: GLFW, GLAD, Dear ImGui (docking branch), stb_image
// ============================================================================

// ---- stb_image (header-only, place stb_image.h next to this file) ----------
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// ---- ImGui & backends -------------------------------------------------------
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

// ---- OpenGL / Window --------------------------------------------------------
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// ---- STL --------------------------------------------------------------------
#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

// ============================================================================
//  CONSTANTS
// ============================================================================
static constexpr int   APP_W = 1440;
static constexpr int   APP_H = 900;
static constexpr int   DEFAULT_CW = 800;   // default canvas width
static constexpr int   DEFAULT_CH = 600;   // default canvas height
static constexpr int   MAX_UNDO = 30;
static constexpr float ZOOM_SPEED = 0.12f;
static constexpr float ZOOM_MIN = 0.05f;
static constexpr float ZOOM_MAX = 32.0f;
static constexpr float BRUSH_SIZE_MAX = 128.0f;

// ============================================================================
//  COLOUR HELPERS
// ============================================================================
struct Color4 { uint8_t r, g, b, a; };

inline Color4 ImVec4ToColor4(const ImVec4& c) {
    return { (uint8_t)(c.x * 255), (uint8_t)(c.y * 255),
             (uint8_t)(c.z * 255), (uint8_t)(c.w * 255) };
}

inline Color4 BlendAlpha(Color4 src, Color4 dst) {
    float sa = src.a / 255.0f;
    float da = dst.a / 255.0f;
    float oa = sa + da * (1.0f - sa);
    if (oa < 1e-5f) return { 0,0,0,0 };
    uint8_t r = (uint8_t)((src.r * sa + dst.r * da * (1.0f - sa)) / oa);
    uint8_t g = (uint8_t)((src.g * sa + dst.g * da * (1.0f - sa)) / oa);
    uint8_t b = (uint8_t)((src.b * sa + dst.b * da * (1.0f - sa)) / oa);
    uint8_t a = (uint8_t)(oa * 255);
    return { r, g, b, a };
}

// ============================================================================
//  PIXEL BUFFER  (owns RGBA data + GL texture)
// ============================================================================
struct PixelBuffer {
    int      width = 0;
    int      height = 0;
    std::vector<uint8_t> pixels;  // RGBA, row-major
    GLuint   texID = 0;

    // ---- lifecycle ----------------------------------------------------------
    void Init(int w, int h, Color4 fill = { 0,0,0,0 }) {
        width = w;
        height = h;
        pixels.assign((size_t)w * h * 4, 0);
        for (int i = 0; i < w * h; ++i) {
            pixels[i * 4 + 0] = fill.r;
            pixels[i * 4 + 1] = fill.g;
            pixels[i * 4 + 2] = fill.b;
            pixels[i * 4 + 3] = fill.a;
        }
        if (texID == 0) glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }

    void Free() {
        if (texID) { glDeleteTextures(1, &texID); texID = 0; }
        pixels.clear(); width = height = 0;
    }

    // ---- pixel access -------------------------------------------------------
    inline Color4 Get(int x, int y) const {
        size_t idx = ((size_t)y * width + x) * 4;
        return { pixels[idx], pixels[idx + 1], pixels[idx + 2], pixels[idx + 3] };
    }

    inline void Set(int x, int y, Color4 c) {
        size_t idx = ((size_t)y * width + x) * 4;
        pixels[idx + 0] = c.r; pixels[idx + 1] = c.g;
        pixels[idx + 2] = c.b; pixels[idx + 3] = c.a;
    }

    // ---- push dirty region to GPU -------------------------------------------
    void UploadFull() {
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
            GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    }

    void UploadRegion(int x, int y, int w, int h) {
        // clamp to texture bounds
        int x1 = std::max(x, 0), y1 = std::max(y, 0);
        int x2 = std::min(x + w, width), y2 = std::min(y + h, height);
        if (x1 >= x2 || y1 >= y2) return;
        int rw = x2 - x1, rh = y2 - y1;

        // Build contiguous sub-image (glPixelStorei unpack could work too)
        std::vector<uint8_t> sub((size_t)rw * rh * 4);
        for (int row = 0; row < rh; ++row) {
            const uint8_t* src = pixels.data() + ((size_t)(y1 + row) * width + x1) * 4;
            uint8_t* dst = sub.data() + (size_t)row * rw * 4;
            std::memcpy(dst, src, (size_t)rw * 4);
        }
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x1, y1, rw, rh,
            GL_RGBA, GL_UNSIGNED_BYTE, sub.data());
    }
};

// ============================================================================
//  LAYER
// ============================================================================
struct Layer {
    std::string  name;
    PixelBuffer  buf;
    bool         visible = true;
    float        opacity = 1.0f;  // 0..1

    Layer() = default;
    Layer(const std::string& n, int w, int h, Color4 fill = { 0,0,0,0 })
        : name(n) {
        buf.Init(w, h, fill);
    }

    ~Layer() { buf.Free(); }

    // Non-copyable (owns GL texture)
    Layer(const Layer&) = delete;
    Layer& operator=(const Layer&) = delete;
    Layer(Layer&&) = default;
    Layer& operator=(Layer&&) = default;
};

// ============================================================================
//  UNDO / REDO   (snapshot-based per stroke)
// ============================================================================
struct UndoState {
    int   layerIndex;
    std::vector<uint8_t> pixels;  // full layer snapshot
};

struct UndoStack {
    std::vector<UndoState> undoStack;
    std::vector<UndoState> redoStack;

    void Push(int layerIdx, const PixelBuffer& buf) {
        if ((int)undoStack.size() >= MAX_UNDO) undoStack.erase(undoStack.begin());
        undoStack.push_back({ layerIdx, buf.pixels });
        redoStack.clear();
    }

    bool CanUndo() const { return !undoStack.empty(); }
    bool CanRedo() const { return !redoStack.empty(); }

    // Returns the state to restore
    UndoState PopUndo(int layerIdx, const PixelBuffer& buf) {
        UndoState top = std::move(undoStack.back()); undoStack.pop_back();
        redoStack.push_back({ layerIdx, buf.pixels });
        return top;
    }

    UndoState PopRedo(int layerIdx, const PixelBuffer& buf) {
        UndoState top = std::move(redoStack.back()); redoStack.pop_back();
        undoStack.push_back({ layerIdx, buf.pixels });
        return top;
    }
};

// ============================================================================
//  CANVAS  (composite texture + layers + view state)
// ============================================================================
struct Canvas {
    int   width = DEFAULT_CW;
    int   height = DEFAULT_CH;
    float zoom = 1.0f;
    ImVec2 pan = { 0.f, 0.f };      // offset in screen pixels

    std::vector<std::unique_ptr<Layer>> layers;
    int   activeLayer = 0;

    // Composite (merged) texture shown in viewport
    PixelBuffer composite;

    UndoStack undo;

    // ---- init ---------------------------------------------------------------
    void Init(int w, int h) {
        width = w; height = h;
        composite.Init(w, h, { 255,255,255,255 });

        // Start with one white background layer + one transparent paint layer
        layers.clear();
        auto bg = std::make_unique<Layer>("Background", w, h, Color4{ 255,255,255,255 });
        auto paint = std::make_unique<Layer>("Layer 1", w, h, Color4{ 0,0,0,0 });
        layers.push_back(std::move(bg));
        layers.push_back(std::move(paint));
        activeLayer = 1;
    }

    // ---- load image into background layer -----------------------------------
    bool LoadImage(const char* path) {
        int iw, ih, ch;
        stbi_set_flip_vertically_on_load(false);
        uint8_t* data = stbi_load(path, &iw, &ih, &ch, 4);
        if (!data) return false;

        width = iw; height = ih;
        // Resize all layers
        layers.clear();
        composite.Init(iw, ih);

        auto bg = std::make_unique<Layer>("Background", iw, ih);
        bg->buf.pixels.assign(data, data + (size_t)iw * ih * 4);
        bg->buf.UploadFull();
        layers.push_back(std::move(bg));

        auto paint = std::make_unique<Layer>("Layer 1", iw, ih, Color4{ 0,0,0,0 });
        layers.push_back(std::move(paint));
        activeLayer = 1;

        stbi_image_free(data);
        RecompositeAll();
        return true;
    }

    // ---- export flat image --------------------------------------------------
    bool ExportPNG(const char* path) {
        RecompositeAll();
        // Flip vertically for standard image orientation (optional)
        std::vector<uint8_t> flipped((size_t)width * height * 4);
        for (int y = 0; y < height; ++y) {
            const uint8_t* src = composite.pixels.data() + (size_t)y * width * 4;
            uint8_t* dst = flipped.data() + (size_t)(height - 1 - y) * width * 4;
            std::memcpy(dst, src, (size_t)width * 4);
        }
        return stbi_write_png(path, width, height, 4, flipped.data(), width * 4) != 0;
    }

    // ---- layer management ---------------------------------------------------
    void AddLayer() {
        char name[32]; snprintf(name, sizeof(name), "Layer %d", (int)layers.size() + 1);
        layers.push_back(std::make_unique<Layer>(name, width, height, Color4{ 0,0,0,0 }));
        activeLayer = (int)layers.size() - 1;
    }

    void DeleteLayer(int idx) {
        if (layers.size() <= 1) return;
        layers.erase(layers.begin() + idx);
        activeLayer = std::clamp(activeLayer, 0, (int)layers.size() - 1);
        RecompositeAll();
    }

    void MoveLayerUp(int idx) {
        if (idx <= 0 || idx >= (int)layers.size()) return;
        std::swap(layers[idx], layers[idx - 1]);
        if (activeLayer == idx) activeLayer = idx - 1;
        else if (activeLayer == idx - 1) activeLayer = idx;
        RecompositeAll();
    }

    void MoveLayerDown(int idx) {
        if (idx < 0 || idx >= (int)layers.size() - 1) return;
        std::swap(layers[idx], layers[idx + 1]);
        if (activeLayer == idx) activeLayer = idx + 1;
        else if (activeLayer == idx + 1) activeLayer = idx;
        RecompositeAll();
    }

    // ---- compositing (bottom → top alpha blend) -----------------------------
    void RecompositeAll() {
        // Start from fully transparent
        std::fill(composite.pixels.begin(), composite.pixels.end(), 0);

        for (auto& layer : layers) {
            if (!layer->visible) continue;
            float opacity = layer->opacity;
            for (int i = 0; i < width * height; ++i) {
                Color4 src = {
                    layer->buf.pixels[i * 4 + 0],
                    layer->buf.pixels[i * 4 + 1],
                    layer->buf.pixels[i * 4 + 2],
                    (uint8_t)(layer->buf.pixels[i * 4 + 3] * opacity)
                };
                Color4 dst = {
                    composite.pixels[i * 4 + 0],
                    composite.pixels[i * 4 + 1],
                    composite.pixels[i * 4 + 2],
                    composite.pixels[i * 4 + 3]
                };
                Color4 out = BlendAlpha(src, dst);
                composite.pixels[i * 4 + 0] = out.r;
                composite.pixels[i * 4 + 1] = out.g;
                composite.pixels[i * 4 + 2] = out.b;
                composite.pixels[i * 4 + 3] = out.a;
            }
        }
        composite.UploadFull();
    }

    // ---- coordinate mapping -------------------------------------------------
    // Convert canvas-window-local mouse pos → canvas pixel coords
    ImVec2 ScreenToCanvas(ImVec2 screenPos, ImVec2 canvasOriginInWindow) const {
        float cx = (screenPos.x - canvasOriginInWindow.x - pan.x) / zoom;
        float cy = (screenPos.y - canvasOriginInWindow.y - pan.y) / zoom;
        return { cx, cy };
    }
};

// ============================================================================
//  DRAWING TOOLS
// ============================================================================
enum class ToolType { Brush, Eraser, Eyedropper, RectSelect, Fill };

struct BrushStroke {
    int   lastX = -1, lastY = -1;
    bool  active = false;

    // Draw a filled circle at (px,py) with radius r onto the active layer
    // Returns the dirty bounding box in pixel coords
    void DrawCircle(Canvas& canvas, int px, int py, int radius, Color4 color, bool erase) {
        Layer* layer = canvas.layers[canvas.activeLayer].get();
        int x0 = px - radius, y0 = py - radius;
        int x1 = px + radius, y1 = py + radius;

        for (int y = std::max(0, y0); y <= std::min(canvas.height - 1, y1); ++y) {
            for (int x = std::max(0, x0); x <= std::min(canvas.width - 1, x1); ++x) {
                int dx = x - px, dy = y - py;
                if (dx * dx + dy * dy <= radius * radius) {
                    if (erase) {
                        layer->buf.Set(x, y, { 0,0,0,0 });
                    }
                    else {
                        Color4 existing = layer->buf.Get(x, y);
                        layer->buf.Set(x, y, BlendAlpha(color, existing));
                    }
                }
            }
        }
        // Upload dirty region
        int margin = 1;
        layer->buf.UploadRegion(x0 - margin, y0 - margin,
            (x1 - x0) + margin * 2, (y1 - y0) + margin * 2);
    }

    // Interpolate between two points (Bresenham-style stamp)
    void DrawLine(Canvas& canvas, int x0, int y0, int x1, int y1,
        int radius, Color4 color, bool erase) {
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        int steps = std::max(dx, dy);
        if (steps == 0) { DrawCircle(canvas, x0, y0, radius, color, erase); return; }
        for (int i = 0; i <= steps; ++i) {
            float t = (float)i / steps;
            int x = (int)(x0 + t * (x1 - x0));
            int y = (int)(y0 + t * (y1 - y0));
            DrawCircle(canvas, x, y, radius, color, erase);
        }
    }
};

// ---- Flood Fill (4-connected) -----------------------------------------------
void FloodFill(Canvas& canvas, int sx, int sy, Color4 fillColor) {
    Layer* layer = canvas.layers[canvas.activeLayer].get();
    if (sx < 0 || sy < 0 || sx >= canvas.width || sy >= canvas.height) return;

    Color4 target = layer->buf.Get(sx, sy);
    if (target.r == fillColor.r && target.g == fillColor.g &&
        target.b == fillColor.b && target.a == fillColor.a) return;

    std::vector<std::pair<int, int>> stack;
    stack.emplace_back(sx, sy);
    while (!stack.empty()) {
        auto [x, y] = stack.back(); stack.pop_back();
        if (x < 0 || y < 0 || x >= canvas.width || y >= canvas.height) continue;
        Color4 cur = layer->buf.Get(x, y);
        if (cur.r != target.r || cur.g != target.g ||
            cur.b != target.b || cur.a != target.a) continue;
        layer->buf.Set(x, y, fillColor);
        stack.emplace_back(x + 1, y); stack.emplace_back(x - 1, y);
        stack.emplace_back(x, y + 1); stack.emplace_back(x, y - 1);
    }
    layer->buf.UploadFull();
}

// ============================================================================
//  APP STATE
// ============================================================================
struct AppState {
    // Tools
    ToolType    activeTool = ToolType::Brush;
    float       brushSize = 10.0f;
    float       brushOpacity = 1.0f;
    ImVec4      fgColor = { 0.0f, 0.0f, 0.0f, 1.0f };  // foreground
    ImVec4      bgColor = { 1.0f, 1.0f, 1.0f, 1.0f };  // background

    // Flags
    bool        isPainting = false;
    bool        isPanning = false;
    ImVec2      lastMousePos = { 0,0 };
    ImVec2      panStart = { 0,0 };
    ImVec2      panOrigin = { 0,0 };

    // UI windows
    bool        showNewDialog = false;
    bool        showExportDialog = false;
    char        loadPathBuf[512] = "";
    char        exportPathBuf[512] = "output.png";
    char        newWidthBuf[8] = "800";
    char        newHeightBuf[8] = "600";

    // Log
    std::vector<std::string> log;

    void Log(const std::string& msg) {
        log.push_back(msg);
        if (log.size() > 200) log.erase(log.begin());
    }
};

// ============================================================================
//  UI HELPERS
// ============================================================================
static void HelpMarker(const char* desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 28.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static bool ToolButton(const char* label, bool selected, ImVec2 size = { 40,40 }) {
    if (selected) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 0.8f));
    }
    bool clicked = ImGui::Button(label, size);
    if (selected) ImGui::PopStyleColor(2);
    return clicked;
}

// ============================================================================
//  DRAW TOOLBAR  (left panel)
// ============================================================================
static void DrawToolbar(AppState& app, Canvas& canvas) {
    ImGui::SetNextWindowSize({ 60, 0 }, ImGuiCond_Always);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("##Toolbar", nullptr, flags);

    ImGui::Spacing();
    if (ToolButton("B##Brush", app.activeTool == ToolType::Brush))       app.activeTool = ToolType::Brush;
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Brush (B)");

    if (ToolButton("E##Eraser", app.activeTool == ToolType::Eraser))      app.activeTool = ToolType::Eraser;
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Eraser (E)");

    if (ToolButton("F##Fill", app.activeTool == ToolType::Fill))        app.activeTool = ToolType::Fill;
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Fill / Bucket (F)");

    if (ToolButton("I##Eye", app.activeTool == ToolType::Eyedropper))  app.activeTool = ToolType::Eyedropper;
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Eyedropper (I)");

    ImGui::Separator();

    // Foreground / Background colour swatches
    ImGui::Spacing();
    ImGui::Text("FG");
    ImGui::ColorButton("##FG", app.fgColor,
        ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, { 40,40 });
    if (ImGui::BeginItemTooltip()) { ImGui::Text("Foreground colour"); ImGui::EndTooltip(); }
    if (ImGui::IsItemClicked()) ImGui::OpenPopup("##FGPicker");
    if (ImGui::BeginPopup("##FGPicker")) {
        ImGui::ColorPicker4("##fgp", &app.fgColor.x,
            ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB);
        ImGui::EndPopup();
    }

    ImGui::Text("BG");
    ImGui::ColorButton("##BG", app.bgColor,
        ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoBorder, { 40,40 });
    if (ImGui::IsItemClicked()) ImGui::OpenPopup("##BGPicker");
    if (ImGui::BeginPopup("##BGPicker")) {
        ImGui::ColorPicker4("##bgp", &app.bgColor.x,
            ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_DisplayRGB);
        ImGui::EndPopup();
    }

    // Swap FG/BG
    if (ImGui::Button("<>", { 40, 20 })) std::swap(app.fgColor, app.bgColor);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Swap FG / BG");

    ImGui::Separator();
    ImGui::Spacing();

    // Brush size slider (vertical label workaround)
    ImGui::Text("Sz");
    ImGui::SetNextItemWidth(40);
    ImGui::SliderFloat("##sz", &app.brushSize, 1.0f, BRUSH_SIZE_MAX, "%.0f",
        ImGuiSliderFlags_Vertical);

    ImGui::Spacing();
    ImGui::Text("Op");
    ImGui::SetNextItemWidth(40);
    ImGui::SliderFloat("##op", &app.brushOpacity, 0.0f, 1.0f, "%.2f",
        ImGuiSliderFlags_Vertical);

    ImGui::End();
}

// ============================================================================
//  DRAW LAYERS PANEL  (right panel)
// ============================================================================
static void DrawLayersPanel(AppState& app, Canvas& canvas) {
    ImGui::Begin("Layers");

    // Add / Delete / Move buttons
    if (ImGui::Button(" + ")) {
        canvas.AddLayer();
        app.Log("Added layer: " + canvas.layers.back()->name);
    }
    ImGui::SameLine();
    if (ImGui::Button(" - ")) {
        if (canvas.layers.size() > 1) {
            std::string n = canvas.layers[canvas.activeLayer]->name;
            canvas.DeleteLayer(canvas.activeLayer);
            canvas.RecompositeAll();
            app.Log("Deleted layer: " + n);
        }
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##up", ImGuiDir_Up)) {
        canvas.MoveLayerUp(canvas.activeLayer);
        app.Log("Layer moved up");
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##dn", ImGuiDir_Down)) {
        canvas.MoveLayerDown(canvas.activeLayer);
        app.Log("Layer moved down");
    }

    ImGui::Separator();

    // List layers (top of stack first)
    for (int i = (int)canvas.layers.size() - 1; i >= 0; --i) {
        Layer* layer = canvas.layers[i].get();

        ImGui::PushID(i);

        // Visibility checkbox
        bool v = layer->visible;
        if (ImGui::Checkbox("##v", &v)) {
            layer->visible = v;
            canvas.RecompositeAll();
        }
        ImGui::SameLine();

        // Layer name + selection highlight
        bool selected = (canvas.activeLayer == i);
        if (selected)
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.26f, 0.59f, 0.98f, 0.4f));

        char buf[64]; snprintf(buf, sizeof(buf), "%s##sel", layer->name.c_str());
        if (ImGui::Selectable(buf, selected, 0, ImVec2(0, 0))) {
            canvas.activeLayer = i;
        }
        if (selected) ImGui::PopStyleColor();

        // Right-click → rename
        if (ImGui::BeginPopupContextItem("##ctx")) {
            static char nameEdit[64] = "";
            if (ImGui::IsWindowAppearing()) {
                snprintf(nameEdit, sizeof(nameEdit), "%s", layer->name.c_str());
            }
            ImGui::SetNextItemWidth(140);
            if (ImGui::InputText("Name##rename", nameEdit, sizeof(nameEdit),
                ImGuiInputTextFlags_EnterReturnsTrue)) {
                layer->name = nameEdit;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Opacity slider
        if (selected) {
            ImGui::Indent(8);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - 8);
            if (ImGui::SliderFloat("Opacity##op", &layer->opacity, 0.0f, 1.0f, "%.2f")) {
                canvas.RecompositeAll();
            }
            ImGui::Unindent(8);
        }

        ImGui::PopID();
    }

    ImGui::End();
}

// ============================================================================
//  DRAW BOTTOM PANEL  (properties + log)
// ============================================================================
static void DrawBottomPanel(AppState& app, Canvas& canvas, ImVec2 mouseCanvasPos) {
    ImGui::Begin("Properties & Log");

    // Image info
    ImGui::Text("Canvas: %d x %d px  |  Zoom: %.0f%%  |  Layers: %d  |  Active: %s",
        canvas.width, canvas.height,
        canvas.zoom * 100.0f,
        (int)canvas.layers.size(),
        canvas.layers[canvas.activeLayer]->name.c_str());

    // Cursor position on canvas
    ImGui::SameLine(0, 20);
    if (mouseCanvasPos.x >= 0 && mouseCanvasPos.y >= 0 &&
        mouseCanvasPos.x < canvas.width && mouseCanvasPos.y < canvas.height) {
        ImGui::Text("Cursor: (%.0f, %.0f)", mouseCanvasPos.x, mouseCanvasPos.y);
    }
    else {
        ImGui::TextDisabled("Cursor: outside canvas");
    }

    ImGui::Separator();

    // Log
    ImGui::BeginChild("##log", ImVec2(0, 0), false,
        ImGuiWindowFlags_HorizontalScrollbar);
    for (auto& entry : app.log)
        ImGui::TextUnformatted(entry.c_str());
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();

    ImGui::End();
}

// ============================================================================
//  DRAW CANVAS WINDOW  (centre panel)
// ============================================================================
static BrushStroke g_stroke;

static void DrawCanvasWindow(AppState& app, Canvas& canvas, ImVec2& outMouseCanvasPos) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Canvas", nullptr,
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::PopStyleVar();

    ImVec2 winPos = ImGui::GetWindowPos();
    ImVec2 winSize = ImGui::GetWindowSize();

    // Adjust for tab-bar / title-bar offset
    ImVec2 contentOrigin = ImGui::GetCursorScreenPos();

    // Checkerboard background (draw via DrawList before the image)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const int   csz = 12;  // checker cell size in pixels
        ImVec4 c1(0.5f, 0.5f, 0.5f, 1.f), c2(0.7f, 0.7f, 0.7f, 1.f);
        int cols = (int)(winSize.x / csz) + 2;
        int rows = (int)(winSize.y / csz) + 2;
        for (int row = 0; row < rows; ++row)
            for (int col = 0; col < cols; ++col) {
                bool dark = (row + col) % 2 == 0;
                ImVec4& cc = dark ? c1 : c2;
                ImVec2 p0 = { contentOrigin.x + col * csz, contentOrigin.y + row * csz };
                ImVec2 p1 = { p0.x + csz, p0.y + csz };
                dl->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(cc));
            }
    }

    // Compute display rect of canvas image
    float dispW = canvas.width * canvas.zoom;
    float dispH = canvas.height * canvas.zoom;
    ImVec2 imgOrigin = {
        contentOrigin.x + canvas.pan.x,
        contentOrigin.y + canvas.pan.y
    };

    // Draw composite texture
    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddImage((ImTextureID)(intptr_t)canvas.composite.texID,
        imgOrigin,
        { imgOrigin.x + dispW, imgOrigin.y + dispH },
        { 0,0 }, { 1,1 });

    // Canvas border
    dl->AddRect(imgOrigin, { imgOrigin.x + dispW, imgOrigin.y + dispH },
        IM_COL32(100, 100, 100, 200), 0.f, 0, 1.f);

    // ---- Input Handling ----
    bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows |
        ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2   mouse = io.MousePos;

    // Map mouse to canvas pixel
    ImVec2 cpos = canvas.ScreenToCanvas(mouse, contentOrigin);
    outMouseCanvasPos = cpos;

    // Zoom (mouse wheel)
    if (hovered && io.MouseWheel != 0.0f) {
        float oldZoom = canvas.zoom;
        canvas.zoom *= (io.MouseWheel > 0 ? (1.f + ZOOM_SPEED) : (1.f - ZOOM_SPEED));
        canvas.zoom = std::clamp(canvas.zoom, ZOOM_MIN, ZOOM_MAX);

        // Zoom toward cursor
        float ratio = canvas.zoom / oldZoom;
        canvas.pan.x = mouse.x - contentOrigin.x - ratio * (mouse.x - contentOrigin.x - canvas.pan.x);
        canvas.pan.y = mouse.y - contentOrigin.y - ratio * (mouse.y - contentOrigin.y - canvas.pan.y);
    }

    // Panning (middle-mouse or Space+LMB)
    bool spaceHeld = ImGui::IsKeyDown(ImGuiKey_Space);
    if (hovered && (io.MouseClicked[2] || (spaceHeld && io.MouseClicked[0]))) {
        app.isPanning = true;
        app.panStart = mouse;
        app.panOrigin = canvas.pan;
    }
    if (app.isPanning) {
        canvas.pan = {
            app.panOrigin.x + (mouse.x - app.panStart.x),
            app.panOrigin.y + (mouse.y - app.panStart.y)
        };
        if (!io.MouseDown[2] && !(spaceHeld && io.MouseDown[0])) {
            app.isPanning = false;
        }
    }

    // Cursor feedback
    if (hovered && !app.isPanning) {
        if (app.activeTool == ToolType::Brush || app.activeTool == ToolType::Eraser)
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        else if (app.activeTool == ToolType::Eyedropper)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        else
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    // Draw brush cursor ring
    if (hovered && !app.isPanning &&
        (app.activeTool == ToolType::Brush || app.activeTool == ToolType::Eraser)) {
        float screenRadius = app.brushSize * 0.5f * canvas.zoom;
        dl->AddCircle(mouse, screenRadius, IM_COL32(255, 255, 255, 200), 32, 1.5f);
        dl->AddCircle(mouse, screenRadius, IM_COL32(0, 0, 0, 200), 32, 0.5f);
    }

    bool canvasBounds = (cpos.x >= 0 && cpos.y >= 0 &&
        cpos.x < canvas.width && cpos.y < canvas.height);

    // ---- TOOL: Brush / Eraser ----
    if (hovered && !app.isPanning &&
        (app.activeTool == ToolType::Brush || app.activeTool == ToolType::Eraser)) {
        bool erase = (app.activeTool == ToolType::Eraser);

        if (io.MouseClicked[0] && canvasBounds) {
            // Start stroke → push undo snapshot
            canvas.undo.Push(canvas.activeLayer, canvas.layers[canvas.activeLayer]->buf);
            app.isPainting = true;
            g_stroke.lastX = (int)cpos.x;
            g_stroke.lastY = (int)cpos.y;

            Color4 c = ImVec4ToColor4(app.fgColor);
            c.a = (uint8_t)(c.a * app.brushOpacity);
            g_stroke.DrawCircle(canvas, g_stroke.lastX, g_stroke.lastY,
                (int)(app.brushSize * 0.5f), c, erase);
            canvas.RecompositeAll();
        }

        if (app.isPainting && io.MouseDown[0] && canvasBounds) {
            int cx = (int)cpos.x, cy = (int)cpos.y;
            if (cx != g_stroke.lastX || cy != g_stroke.lastY) {
                Color4 c = ImVec4ToColor4(app.fgColor);
                c.a = (uint8_t)(c.a * app.brushOpacity);
                g_stroke.DrawLine(canvas, g_stroke.lastX, g_stroke.lastY, cx, cy,
                    (int)(app.brushSize * 0.5f), c, erase);
                g_stroke.lastX = cx; g_stroke.lastY = cy;
                canvas.RecompositeAll();
            }
        }

        if (!io.MouseDown[0]) app.isPainting = false;
    }

    // ---- TOOL: Eyedropper ----
    if (hovered && app.activeTool == ToolType::Eyedropper &&
        io.MouseClicked[0] && canvasBounds) {
        Color4 picked = canvas.composite.Get((int)cpos.x, (int)cpos.y);
        app.fgColor = { picked.r / 255.f, picked.g / 255.f, picked.b / 255.f, picked.a / 255.f };
        app.Log("Colour picked: RGBA("
            + std::to_string(picked.r) + ","
            + std::to_string(picked.g) + ","
            + std::to_string(picked.b) + ","
            + std::to_string(picked.a) + ")");
    }

    // ---- TOOL: Fill ----
    if (hovered && app.activeTool == ToolType::Fill &&
        io.MouseClicked[0] && canvasBounds) {
        canvas.undo.Push(canvas.activeLayer, canvas.layers[canvas.activeLayer]->buf);
        Color4 c = ImVec4ToColor4(app.fgColor);
        c.a = (uint8_t)(c.a * app.brushOpacity);
        FloodFill(canvas, (int)cpos.x, (int)cpos.y, c);
        canvas.RecompositeAll();
        app.Log("Flood fill applied.");
    }

    // ---- Keyboard shortcuts ----
    if (!ImGui::GetIO().WantCaptureKeyboard) {
        if (ImGui::IsKeyPressed(ImGuiKey_B)) app.activeTool = ToolType::Brush;
        if (ImGui::IsKeyPressed(ImGuiKey_E)) app.activeTool = ToolType::Eraser;
        if (ImGui::IsKeyPressed(ImGuiKey_F)) app.activeTool = ToolType::Fill;
        if (ImGui::IsKeyPressed(ImGuiKey_I)) app.activeTool = ToolType::Eyedropper;
    }

    // Undo / Redo
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z)) {
        if (canvas.undo.CanUndo()) {
            UndoState s = canvas.undo.PopUndo(canvas.activeLayer,
                canvas.layers[canvas.activeLayer]->buf);
            canvas.layers[s.layerIndex]->buf.pixels = std::move(s.pixels);
            canvas.layers[s.layerIndex]->buf.UploadFull();
            canvas.RecompositeAll();
            app.Log("Undo");
        }
    }
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y)) {
        if (canvas.undo.CanRedo()) {
            UndoState s = canvas.undo.PopRedo(canvas.activeLayer,
                canvas.layers[canvas.activeLayer]->buf);
            canvas.layers[s.layerIndex]->buf.pixels = std::move(s.pixels);
            canvas.layers[s.layerIndex]->buf.UploadFull();
            canvas.RecompositeAll();
            app.Log("Redo");
        }
    }

    ImGui::End();
}

// ============================================================================
//  DRAW MENU BAR
// ============================================================================
static void DrawMenuBar(AppState& app, Canvas& canvas) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Canvas...", "Ctrl+N"))
                app.showNewDialog = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Open Image...")) {
                // Simple: use InputText path field
                ImGui::OpenPopup("##open_path_popup");
            }
            // Inline popup for open path
            ImGui::SetNextWindowSize({ 400, 80 });
            if (ImGui::BeginPopupModal("##open_path_popup", nullptr,
                ImGuiWindowFlags_NoTitleBar)) {
                ImGui::Text("Image path:");
                ImGui::SetNextItemWidth(-60);
                ImGui::InputText("##lp", app.loadPathBuf, sizeof(app.loadPathBuf));
                ImGui::SameLine();
                if (ImGui::Button("Load")) {
                    if (canvas.LoadImage(app.loadPathBuf))
                        app.Log(std::string("Loaded: ") + app.loadPathBuf);
                    else
                        app.Log(std::string("ERROR loading: ") + app.loadPathBuf);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Export as PNG...", "Ctrl+E"))
                app.showExportDialog = true;

            ImGui::Separator();
            if (ImGui::MenuItem("Quit", "Alt+F4"))
                glfwSetWindowShouldClose(glfwGetCurrentContext(), true);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, canvas.undo.CanUndo())) {
                if (canvas.undo.CanUndo()) {
                    UndoState s = canvas.undo.PopUndo(canvas.activeLayer,
                        canvas.layers[canvas.activeLayer]->buf);
                    canvas.layers[s.layerIndex]->buf.pixels = std::move(s.pixels);
                    canvas.layers[s.layerIndex]->buf.UploadFull();
                    canvas.RecompositeAll();
                    app.Log("Undo");
                }
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, canvas.undo.CanRedo())) {
                if (canvas.undo.CanRedo()) {
                    UndoState s = canvas.undo.PopRedo(canvas.activeLayer,
                        canvas.layers[canvas.activeLayer]->buf);
                    canvas.layers[s.layerIndex]->buf.pixels = std::move(s.pixels);
                    canvas.layers[s.layerIndex]->buf.UploadFull();
                    canvas.RecompositeAll();
                    app.Log("Redo");
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Clear Active Layer")) {
                canvas.undo.Push(canvas.activeLayer, canvas.layers[canvas.activeLayer]->buf);
                auto& pb = canvas.layers[canvas.activeLayer]->buf;
                std::fill(pb.pixels.begin(), pb.pixels.end(), 0);
                pb.UploadFull();
                canvas.RecompositeAll();
                app.Log("Layer cleared.");
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Zoom In", "Ctrl++")) canvas.zoom = std::min(canvas.zoom * 1.25f, ZOOM_MAX);
            if (ImGui::MenuItem("Zoom Out", "Ctrl+-")) canvas.zoom = std::max(canvas.zoom / 1.25f, ZOOM_MIN);
            if (ImGui::MenuItem("Fit to Window")) { canvas.zoom = 1.0f; canvas.pan = { 0,0 }; }
            if (ImGui::MenuItem("Reset View")) { canvas.zoom = 1.0f; canvas.pan = { 0,0 }; }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Layer")) {
            if (ImGui::MenuItem("Add Layer")) { canvas.AddLayer(); app.Log("Layer added."); }
            if (ImGui::MenuItem("Delete Layer", nullptr, false, canvas.layers.size() > 1)) {
                canvas.DeleteLayer(canvas.activeLayer);
                canvas.RecompositeAll();
                app.Log("Layer deleted.");
            }
            ImGui::EndMenu();
        }

        // Zoom indicator in menu bar
        ImGui::SetCursorPosX(ImGui::GetIO().DisplaySize.x - 120);
        ImGui::Text("Zoom: %.0f%%  ", canvas.zoom * 100.f);

        ImGui::EndMainMenuBar();
    }

    // ---- New Canvas dialog ----
    if (app.showNewDialog) ImGui::OpenPopup("New Canvas");
    ImGui::SetNextWindowSize({ 320, 140 }, ImGuiCond_Always);
    if (ImGui::BeginPopupModal("New Canvas", &app.showNewDialog,
        ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Width:");  ImGui::SameLine(80);
        ImGui::SetNextItemWidth(100);
        ImGui::InputText("##nw", app.newWidthBuf, sizeof(app.newWidthBuf),
            ImGuiInputTextFlags_CharsDecimal);
        ImGui::Text("Height:"); ImGui::SameLine(80);
        ImGui::SetNextItemWidth(100);
        ImGui::InputText("##nh", app.newHeightBuf, sizeof(app.newHeightBuf),
            ImGuiInputTextFlags_CharsDecimal);

        ImGui::Spacing();
        if (ImGui::Button("Create", { 80, 0 })) {
            int nw = std::max(1, atoi(app.newWidthBuf));
            int nh = std::max(1, atoi(app.newHeightBuf));
            canvas.Init(nw, nh);
            app.Log("New canvas: " + std::to_string(nw) + "x" + std::to_string(nh));
            app.showNewDialog = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", { 80, 0 })) {
            app.showNewDialog = false; ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    // ---- Export dialog ----
    if (app.showExportDialog) ImGui::OpenPopup("Export PNG");
    ImGui::SetNextWindowSize({ 380, 100 }, ImGuiCond_Always);
    if (ImGui::BeginPopupModal("Export PNG", &app.showExportDialog,
        ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Save path:");
        ImGui::SetNextItemWidth(-70);
        ImGui::InputText("##ep", app.exportPathBuf, sizeof(app.exportPathBuf));
        ImGui::SameLine();
        if (ImGui::Button("Save", { 60,0 })) {
            if (canvas.ExportPNG(app.exportPathBuf))
                app.Log(std::string("Exported: ") + app.exportPathBuf);
            else
                app.Log(std::string("ERROR exporting: ") + app.exportPathBuf);
            app.showExportDialog = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) { app.showExportDialog = false; ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

// ============================================================================
//  SETUP DOCKSPACE  (covers whole window below menu bar)
// ============================================================================
static void SetupDockSpace() {
    ImGuiWindowFlags dockFlags =
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    float menuH = ImGui::GetFrameHeight();   // approx menu bar height
    ImGui::SetNextWindowPos({ vp->WorkPos.x, vp->WorkPos.y + menuH });
    ImGui::SetNextWindowSize({ vp->WorkSize.x, vp->WorkSize.y - menuH });
    ImGui::SetNextWindowViewport(vp->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

    ImGui::Begin("##MainDockSpace", nullptr, dockFlags);
    ImGui::PopStyleVar(3);

    ImGuiID dockID = ImGui::GetID("MainDS");
    ImGui::DockSpace(dockID, ImVec2(0, 0),
        ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();
}

// ============================================================================
//  MAIN
// ============================================================================
int main() {
    // ---- GLFW ---------------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(APP_W, APP_H, "Photoshop Clone  —  C++/OpenGL", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // V-sync
    gladLoadGL();
    glViewport(0, 0, APP_W, APP_H);

    // ---- ImGui --------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // ---- Style --------------------------------------------------------------
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.f;
    style.FrameRounding = 3.f;
    style.ScrollbarRounding = 3.f;
    style.GrabRounding = 3.f;
    style.TabRounding = 3.f;
    style.WindowBorderSize = 1.f;
    style.FramePadding = { 6.f, 4.f };

    // Photoshop-ish dark palette
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.50f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.50f);
    colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.50f);
    colors[ImGuiCol_Separator] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.12f, 0.96f);
    colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 0.60f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // ---- Application State --------------------------------------------------
    AppState app;
    Canvas   canvas;
    canvas.Init(DEFAULT_CW, DEFAULT_CH);
    app.Log("Photoshop Clone started. Canvas: " +
        std::to_string(DEFAULT_CW) + "x" + std::to_string(DEFAULT_CH));
    app.Log("Tools: B=Brush  E=Eraser  F=Fill  I=Eyedropper");
    app.Log("Pan: Space+LMB or Middle Mouse.  Zoom: Mouse Wheel.");
    app.Log("Undo: Ctrl+Z   Redo: Ctrl+Y");

    // Canvas pan starts centred
    canvas.pan = { 40.f, 40.f };

    bool firstFrame = true;

    // ---- Main Loop ----------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Clear
        glClearColor(0.10f, 0.10f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Menu bar
        DrawMenuBar(app, canvas);

        // DockSpace
        SetupDockSpace();

        // Build default dock layout on first frame
        if (firstFrame) {
            firstFrame = false;
            ImGuiID dockID = ImGui::GetID("MainDS");
            ImGui::DockBuilderRemoveNode(dockID);
            ImGui::DockBuilderAddNode(dockID,
                ImGuiDockNodeFlags_DockSpace | ImGuiDockNodeFlags_PassthruCentralNode);

            ImVec2 vp = ImGui::GetMainViewport()->WorkSize;
            ImGui::DockBuilderSetNodeSize(dockID, vp);

            ImGuiID left, centre_right, right, centre, bottom;

            // Split: left toolbar  |  rest
            ImGui::DockBuilderSplitNode(dockID, ImGuiDir_Left, 0.05f, &left, &centre_right);
            // Split rest: centre  |  right panel
            ImGui::DockBuilderSplitNode(centre_right, ImGuiDir_Right, 0.18f, &right, &centre);
            // Split centre: canvas  |  bottom
            ImGui::DockBuilderSplitNode(centre, ImGuiDir_Down, 0.16f, &bottom, &centre);

            ImGui::DockBuilderDockWindow("##Toolbar", left);
            ImGui::DockBuilderDockWindow("Canvas", centre);
            ImGui::DockBuilderDockWindow("Layers", right);
            ImGui::DockBuilderDockWindow("Properties & Log", bottom);
            ImGui::DockBuilderFinish(dockID);
        }

        ImVec2 mouseCanvasPos = { -1,-1 };

        // Panels
        DrawToolbar(app, canvas);
        DrawLayersPanel(app, canvas);
        DrawCanvasWindow(app, canvas, mouseCanvasPos);
        DrawBottomPanel(app, canvas, mouseCanvasPos);

        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // ---- Cleanup ------------------------------------------------------------
    canvas.composite.Free();
    canvas.layers.clear();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}