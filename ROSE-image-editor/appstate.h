#pragma once

#include <cstdarg>
#include <string>
#include <vector>

#include "imgui.h"
#include "appcommands.h"

struct GLFWwindow;

// Central application UI state shared by the menu bar, panels and dialogs.
// Menus and global shortcuts push Cmd values; Dispatch() routes them.
namespace App
{
    // ---- Panel visibility (Window menu checkboxes) ----
    extern bool ShowTools;
    extern bool ShowLayers;
    extern bool ShowColor;
    extern bool ShowHistory;
    extern bool ShowProperties;
    extern bool ShowViewport;
    extern bool Show3D;
    extern bool ShowActions;
    extern bool ShowAdjustments;
    extern bool ShowBrushSettings;
    extern bool ShowBrushes;
    extern bool ShowChannels;
    extern bool ShowCharacter;
    extern bool ShowNavigator;
    extern bool ShowParagraph;
    extern bool ShowPaths;
    extern bool ShowSwatches;
    extern bool ShowTimeline;
    extern bool ShowToolSettings;
    extern bool ShowInfo;

    // ---- View overlays (View > Show) ----
    extern bool ShowGrid;
    extern bool ShowRulers;
    extern bool ShowGuides;
    extern bool ShowTargetPath;
    extern bool ShowCanvasGuides;
    extern bool ShowPixelGrid;
    extern bool ShowLayerEdges;
    extern bool ShowSelectionEdges;

    // ---- View / Screen state ----
    extern int   ScreenMode;        // 0 = Standard, 1 = Full with Menu Bar, 2 = Full Screen
    extern bool  ShowProofColors;
    extern bool  ShowGamutWarning;
    extern bool  SnapEnabled;
    extern bool  SnapToGuides;
    extern bool  SnapToGrid;
    extern bool  SnapToLayers;
    extern bool  SnapToSlices;
    extern bool  SnapToDocumentBounds;
    extern bool  GuidesLocked;
    extern bool  ApplicationFrame;
    extern bool  ShowOptions;

    // ---- Dialog visibility ----
    extern bool ShowNewDocument;
    extern bool ShowPreferences;
    extern bool ShowKeyboardShortcuts;
    extern bool ShowAbout;

    // ---- Preferences ----
    extern int   PrefTab;
    extern bool  PrefThemeDark;
    extern bool  PrefVSync;
    extern bool  PrefShowFps;

    // ---- Recent files (most recent first) ----
    extern std::vector<std::string> RecentFiles;

    // ---- One-line status / feedback shown on the menu bar ----
    extern std::string Status;

    // ---- GLFW window handle (needed for File > Exit) ----
    void SetWindow(GLFWwindow* window);
    GLFWwindow* Window();

    // ---- Command pump ----
    void Push(Cmd cmd);
    void Dispatch();

    // ---- Global keyboard shortcut handler ----
    void HandleGlobalShortcuts();

    // ---- Helpers ----
    void          SetStatus(const char* fmt, ...);
    void          AddRecent(const std::string& path);
    const char*   CmdName(Cmd cmd);
    bool          HasDocument();
    bool          HasSelection();

    // ---- Shortcut table (single source for the handler and the Help dialog) ----
    struct ShortcutEntry
    {
        Cmd         cmd;
        const char* label;      // human-readable command name
        const char* shortcut;   // display text, e.g. "Ctrl+N"
        ImGuiKey    key;
        bool        ctrl;
        bool        shift;
        bool        alt;
        bool        showInHelp; // false = hidden alternate binding (e.g. Ctrl+Y for Redo)
    };
    const ShortcutEntry* GetShortcuts(int* count);
}