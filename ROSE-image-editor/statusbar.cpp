#include "statusbar.h"

#include <cstdio>
#include <vector>

#include "imgui_internal.h"
#include "appstate.h"
#include "workspace.h"

namespace
{
    constexpr int kMaxKeys = 3;
}

float StatusBar::GetHeight()
{
    // A single-line bar: font height + vertical frame padding, exactly the same
    // height as the top menu bar so both bars look symmetrical.
    return ImGui::GetFrameHeight();
}

// Reserve the bottom work-area inset once per frame. ImGui uses this accumulator
// to compute the viewport work rect on the next frame, which is what
// DockSpaceOverViewport() / a central window sized to the work area uses to
// avoid overlapping the bar. This is the same mechanism BeginMainMenuBar() uses
// for the top menu bar (BuildWorkInsetMin), just mirrored for the bottom edge.
void StatusBar::ReserveBottomSpace()
{
    static int s_LastReservedFrame = -1;
    if (s_LastReservedFrame == ImGui::GetFrameCount())
        return; // already reserved this frame (idempotent)
    s_LastReservedFrame = ImGui::GetFrameCount();

    ImGuiViewportP* viewport = (ImGuiViewportP*)ImGui::GetMainViewport();
    viewport->BuildWorkInsetMax.y += GetHeight();
}

void StatusBar::RenderStatusBar()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float height = GetHeight();

    // Pin the bar to the absolute bottom edge of the viewport, spanning its full
    // width. Using the raw viewport rect (not the work rect) guarantees the bar
    // stays glued to the bottom edge regardless of resizing or reservations.
    const float barY = viewport->Pos.y + viewport->Size.y - height;
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, barY));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, height));
    ImGui::SetNextWindowViewport(viewport->ID);

    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 2.0f));

    if (ImGui::Begin("##StatusBar", nullptr, flags))
    {
        const ImGuiIO& io = ImGui::GetIO();
        const ImGuiStyle& style = ImGui::GetStyle();

        // ---------------------------------------------------------------
        // Left area: zoom level | image resolution
        // ---------------------------------------------------------------
        char zoomText[64];
        if (App::HasDocument())
            snprintf(zoomText, sizeof(zoomText), "Zoom: %.0f%%", Workspace::GetZoom() * 100.0f);
        else
            snprintf(zoomText, sizeof(zoomText), "Zoom: --");

        ImGui::TextUnformatted(zoomText);
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();

        char resText[64];
        if (App::HasDocument())
            snprintf(resText, sizeof(resText), "%d x %d px",
                     Workspace::GetImageWidth(), Workspace::GetImageHeight());
        else
            snprintf(resText, sizeof(resText), "No image loaded");

        ImGui::TextUnformatted(resText);

        // ---------------------------------------------------------------
        // Right area: pressed keys | FPS, anchored to the far-right edge.
        //
        // Compute the exact combined width of every element that will be
        // rendered (including the ImGui item spacing the SameLine() calls
        // insert between them), then back the cursor off that distance from
        // the right edge of the window minus its horizontal padding. This is
        // resolution-independent true right alignment (no left-side margins).
        // ---------------------------------------------------------------
        const std::string keys = GetPressedKeysString();

        char fpsText[64];
        snprintf(fpsText, sizeof(fpsText), "%.1f FPS", io.Framerate);

        const float keysWidth = keys.empty() ? 0.0f : ImGui::CalcTextSize(keys.c_str()).x;
        const float sepWidth  = ImGui::CalcTextSize("|").x;
        const float fpsWidth  = ImGui::CalcTextSize(fpsText).x;
        const float spacing   = style.ItemSpacing.x;

        // keys <spacing> | <spacing> FPS  (the separator only renders when
        // there is at least one pressed key, so fold its spacing in as well).
        const float rightElementsWidth =
            (keys.empty() ? 0.0f : keysWidth + spacing + sepWidth + spacing) + fpsWidth;

        // True right alignment: cursor starts so that the right edge of the
        // last element lands exactly at the window's content right edge.
        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - rightElementsWidth - style.WindowPadding.x);

        if (!keys.empty())
        {
            ImGui::TextUnformatted(keys.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("|");
            ImGui::SameLine();
        }
        ImGui::TextUnformatted(fpsText);
    }
    ImGui::End();

    ImGui::PopStyleVar(3);

    // Report our height into the work area so central content avoids us.
    ReserveBottomSpace();
}

bool StatusBar::IsModifier(ImGuiKey key)
{
    return key == ImGuiKey_LeftCtrl  || key == ImGuiKey_RightCtrl  ||
           key == ImGuiKey_LeftShift || key == ImGuiKey_RightShift ||
           key == ImGuiKey_LeftAlt   || key == ImGuiKey_RightAlt;
}

const char* StatusBar::KeyDisplayName(ImGuiKey key)
{
    switch (key)
    {
    case ImGuiKey_LeftCtrl:   case ImGuiKey_RightCtrl:   return "Ctrl";
    case ImGuiKey_LeftShift:  case ImGuiKey_RightShift:  return "Shift";
    case ImGuiKey_LeftAlt:    case ImGuiKey_RightAlt:    return "Alt";
    case ImGuiKey_Space:      return "Space";
    case ImGuiKey_Enter:      return "Enter";
    case ImGuiKey_Tab:        return "Tab";
    case ImGuiKey_Escape:     return "Esc";
    case ImGuiKey_Backspace:  return "Backspace";
    case ImGuiKey_Delete:     return "Delete";
    case ImGuiKey_Insert:     return "Insert";
    case ImGuiKey_Home:       return "Home";
    case ImGuiKey_End:        return "End";
    case ImGuiKey_PageUp:     return "PageUp";
    case ImGuiKey_PageDown:   return "PageDown";
    case ImGuiKey_UpArrow:    return "Up";
    case ImGuiKey_DownArrow:  return "Down";
    case ImGuiKey_LeftArrow:  return "Left";
    case ImGuiKey_RightArrow: return "Right";
    case ImGuiKey_PrintScreen: return "PrintScreen";
    default: break;
    }

    if (key >= ImGuiKey_0 && key <= ImGuiKey_9)
    {
        static char buf[2];
        buf[0] = (char)('0' + (key - ImGuiKey_0));
        buf[1] = '\0';
        return buf;
    }
    if (key >= ImGuiKey_A && key <= ImGuiKey_Z)
    {
        static char buf[2];
        buf[0] = (char)('A' + (key - ImGuiKey_A));
        buf[1] = '\0';
        return buf;
    }
    if (key >= ImGuiKey_F1 && key <= ImGuiKey_F12)
    {
        static char buf[4];
        snprintf(buf, sizeof(buf), "F%d", 1 + (key - ImGuiKey_F1));
        return buf;
    }

    return ImGui::GetKeyName(key);
}

std::string StatusBar::GetPressedKeysString()
{
    const ImGuiIO& io = ImGui::GetIO();
    std::vector<const char*> parts;
    parts.reserve(kMaxKeys);

    // 1) Modifiers first, in canonical order: Ctrl, Shift, Alt.
    const ImGuiKey modifierPairs[][2] =
    {
        { ImGuiKey_LeftCtrl,  ImGuiKey_RightCtrl  },
        { ImGuiKey_LeftShift, ImGuiKey_RightShift },
        { ImGuiKey_LeftAlt,   ImGuiKey_RightAlt   },
    };
    for (const auto& pair : modifierPairs)
    {
        if ((int)parts.size() >= kMaxKeys)
            break;
        if (ImGui::IsKeyDown(pair[0]) || ImGui::IsKeyDown(pair[1]))
            parts.push_back(KeyDisplayName(pair[0]));
    }

    // 2) The primary (non-modifier) key, if we still have room.
    if ((int)parts.size() < kMaxKeys)
    {
        for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_NamedKey_END; ++key)
        {
            const ImGuiKey k = (ImGuiKey)key;
            if (IsModifier(k))
                continue;
            if (ImGui::IsKeyDown(k))
            {
                parts.push_back(KeyDisplayName(k));
                break; // only report the first primary key
            }
        }
    }

    // 3) Join with " + ", capped at kMaxKeys.
    std::string result;
    for (size_t i = 0; i < parts.size() && i < (size_t)kMaxKeys; ++i)
    {
        if (i > 0)
            result += " + ";
        result += parts[i];
    }
    return result;
}