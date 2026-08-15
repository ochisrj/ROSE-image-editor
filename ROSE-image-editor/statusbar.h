#pragma once

#include <string>

#include "imgui.h"

// Main bottom status bar for ROSE-image-editor.
//
// Mirrors the top Main Menu Bar: a full-width bar pinned to the bottom edge of
// the primary viewport that shows live metrics (zoom, image resolution) on the
// left and live input state (pressed key combination, FPS) on the right.
//
// Layout reservation: call ReserveBottomSpace() BEFORE drawing the central
// content (image viewer / dockspace) and Draw() LAST so the bar renders on top.
// ReserveBottomSpace() shrinks the main viewport's work area by the bar height,
// which is exactly what DockSpaceOverViewport() / a central window sized to the
// work area needs to avoid overlapping the bar.
class StatusBar
{
public:
    // Full pipeline: reserve space, then draw the bar itself.
    static void Draw();

    // Shrink the main viewport work area so the central content does not
    // extend underneath the status bar.
    static void ReserveBottomSpace();

    // Height of the bar (== one ImGui frame height). Used by ReserveBottomSpace
    // and by any central window that wants to size itself above the bar.
    static float GetHeight();

    // Realtime pressed-key detector. Returns e.g. "Ctrl + Shift + Z" or
    // "Alt + F4". Modifiers come first (Ctrl, Shift, Alt), then the primary
    // key. Capped at 3 keys total, joined with " + ".
    static std::string GetPressedKeysString();

private:
    // Friendly display name for a key (letters, digits, F-keys, arrows, etc.).
    static const char* KeyDisplayName(ImGuiKey key);
    static bool        IsModifier(ImGuiKey key);
};