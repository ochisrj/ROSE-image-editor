#include "filemenu.h"

#include <filesystem>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "appstate.h"
#include "appcommands.h"
#include "imageviewer.h"

void FileMenu::DrawMenu(GLFWwindow* window)
{
    App::SetWindow(window);

    if (ImGui::BeginMenu("File"))
    {
        // ---- New / Open ----
        if (ImGui::MenuItem("New...", "Ctrl+N"))
            App::Push(Cmd::FileNew);
        if (ImGui::MenuItem("Open...", "Ctrl+O"))
            App::Push(Cmd::FileOpen);
        if (ImGui::MenuItem("Browse in Bridge...", "Alt+Ctrl+O"))
            App::Push(Cmd::FileBrowseInBridge);
        if (ImGui::MenuItem("Open As...", "Alt+Shift+Ctrl+O"))
            App::Push(Cmd::FileOpenAs);
        if (ImGui::MenuItem("Open as Smart Object..."))
            App::Push(Cmd::FileOpenAsSmartObject);

        // ---- Open Recent ----
        if (ImGui::BeginMenu("Open Recent"))
        {
            if (App::RecentFiles.empty())
            {
                ImGui::TextDisabled("No recent files");
            }
            else
            {
                for (const std::string& path : App::RecentFiles)
                {
                    const std::string name = std::filesystem::path(path).filename().string();
                    if (ImGui::MenuItem(name.c_str()))
                    {
                        ImageViewer::LoadFile(path);
                        App::AddRecent(path);
                        App::SetStatus("Opened %s", name.c_str());
                    }
                    if (ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", path.c_str());
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Clear Recent File List", nullptr, false, !App::RecentFiles.empty()))
            {
                App::RecentFiles.clear();
                App::SetStatus("Recent file list cleared.");
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Close / Save ----
        if (ImGui::MenuItem("Close", "Ctrl+W", false, App::HasDocument()))
            App::Push(Cmd::FileClose);
        if (ImGui::MenuItem("Close All", "Alt+Ctrl+W", false, App::HasDocument()))
            App::Push(Cmd::FileCloseAll);
        if (ImGui::MenuItem("Close and Go to Bridge...", "Shift+Ctrl+W"))
            App::Push(Cmd::FileCloseAndGoToBridge);
        if (ImGui::MenuItem("Save", "Ctrl+S", false, App::HasDocument()))
            App::Push(Cmd::FileSave);
        if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, App::HasDocument()))
            App::Push(Cmd::FileSaveAs);
        if (ImGui::MenuItem("Save a Copy...", "Alt+Ctrl+S", false, App::HasDocument()))
            App::Push(Cmd::FileSaveCopy);
        if (ImGui::MenuItem("Revert", "F12", false, App::HasDocument()))
            App::Push(Cmd::FileRevert);

        ImGui::Separator();

        // ---- Export ----
        if (ImGui::BeginMenu("Export"))
        {
            if (ImGui::MenuItem("Quick Export as PNG", nullptr, false, App::HasDocument()))
                App::Push(Cmd::FileQuickExportPNG);
            if (ImGui::MenuItem("Export As...", "Alt+Shift+Ctrl+W", false, App::HasDocument()))
                App::Push(Cmd::FileExportAs);
            if (ImGui::MenuItem("Export Preferences..."))
                App::Push(Cmd::FileExportPreferences);
            if (ImGui::MenuItem("Save for Web (Legacy)...", "Alt+Shift+Ctrl+S", false, App::HasDocument()))
                App::Push(Cmd::FileSaveForWeb);
            if (ImGui::MenuItem("Render Video...", nullptr, false, App::HasDocument()))
                App::Push(Cmd::FileRenderVideo);
            ImGui::EndMenu();
        }

        // ---- Generate ----
        if (ImGui::BeginMenu("Generate"))
        {
            if (ImGui::MenuItem("Image Assets"))
                App::Push(Cmd::FileGenerateImageAssets);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Place ----
        if (ImGui::MenuItem("Place Embedded..."))
            App::Push(Cmd::FilePlaceEmbedded);
        if (ImGui::MenuItem("Place Linked..."))
            App::Push(Cmd::FilePlaceLinked);

        ImGui::Separator();

        if (ImGui::MenuItem("Package..."))
            App::Push(Cmd::FilePackage);

        ImGui::Separator();

        // ---- Automate ----
        if (ImGui::BeginMenu("Automate"))
        {
            if (ImGui::MenuItem("Batch..."))
                App::Push(Cmd::FileBatch);
            if (ImGui::MenuItem("Create Droplet..."))
                App::Push(Cmd::FileCreateDroplet);
            if (ImGui::MenuItem("Crop and Straighten Photos", nullptr, false, App::HasDocument()))
                App::Push(Cmd::FileCropAndStraightenPhotos);
            if (ImGui::MenuItem("Contact Sheet II..."))
                App::Push(Cmd::FileContactSheetII);
            if (ImGui::MenuItem("PDF Presentation..."))
                App::Push(Cmd::FilePDFPresentation);
            if (ImGui::MenuItem("Photomerge..."))
                App::Push(Cmd::FilePhotomerge);
            ImGui::EndMenu();
        }

        // ---- Scripts ----
        if (ImGui::BeginMenu("Scripts"))
        {
            if (ImGui::MenuItem("Image Processor..."))
                App::Push(Cmd::FileImageProcessor);
            if (ImGui::MenuItem("Delete All Empty Layers"))
                App::Push(Cmd::FileDeleteAllEmptyLayers);
            if (ImGui::MenuItem("Load Files into Stack..."))
                App::Push(Cmd::FileLoadFilesIntoStack);
            if (ImGui::MenuItem("Browse..."))
                App::Push(Cmd::FileScriptsBrowse);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        // ---- Print / Exit ----
        if (ImGui::MenuItem("Print...", "Ctrl+P", false, App::HasDocument()))
            App::Push(Cmd::FilePrint);
        if (ImGui::MenuItem("Print One Copy", "Alt+Shift+Ctrl+P", false, App::HasDocument()))
            App::Push(Cmd::FilePrintOneCopy);

        ImGui::Separator();

        if (ImGui::MenuItem("Exit", "Alt+F4"))
            App::Push(Cmd::FileExit);

        ImGui::EndMenu();
    }
}

void FileMenu::DrawWindow()
{
    if (App::ShowNewDocument)
    {
        ImGui::OpenPopup("New Document");
        App::ShowNewDocument = false;
    }

    bool open = true;
    if (ImGui::BeginPopupModal("New Document", &open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static int width = 1920, height = 1080;
        static int background = 0;
        static const char* const bgNames[] = { "White", "Transparent", "Background Color" };

        ImGui::SetNextItemWidth(220);
        ImGui::InputInt("Width", &width);
        ImGui::SetNextItemWidth(220);
        ImGui::InputInt("Height", &height);
        if (width < 1)  width = 1;
        if (height < 1) height = 1;
        ImGui::SetNextItemWidth(220);
        ImGui::Combo("Background", &background, bgNames, 3);

        ImGui::Separator();
        if (ImGui::Button("Create", ImVec2(110, 0)))
        {
            App::SetStatus("New document %dx%d (%s)", width, height, bgNames[background]);
            App::ShowNewDocument = false;
            open = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(110, 0)))
        {
            App::ShowNewDocument = false;
            open = false;
        }
        ImGui::EndPopup();
    }
}