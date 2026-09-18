#include "LanguageTest.h"
#include "imgui.h"
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#endif

namespace LanguageTest
{
    bool    Show = false;
    ImFont* MainFont = nullptr;
    ImFont* ThaiFont = nullptr;
    ImFont* CustomFont = nullptr;
    float   FontSize = 16.0f;
    char    CustomPath[1024] = "RD CHULAJARUEK.ttf";
    std::string LoadError;

    void SetFonts(ImFont* mainFont, ImFont* thaiFont)
    {
        MainFont = mainFont;
        ThaiFont = thaiFont;
        if (!CustomFont) CustomFont = thaiFont;
        // init CustomPath from thai font if needed (already RD)
    }

    static std::string WideToUtf8(const wchar_t* w)
    {
        if (!w || !*w) return {};
        int n = WideCharToMultiByte(CP_UTF8, 0, w, -1, nullptr, 0, nullptr, nullptr);
        std::string s(n - 1, 0);
        WideCharToMultiByte(CP_UTF8, 0, w, -1, s.data(), n, nullptr, nullptr);
        return s;
    }

    static bool PickTtfFile(std::string& out)
    {
#ifdef _WIN32
        wchar_t buf[MAX_PATH * 4] = {};
        // copy current CustomPath into buf for initial dir
        OPENFILENAMEW ofn{};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = (HWND)ImGui::GetMainViewport()->PlatformHandleRaw;
        ofn.lpstrFilter = L"Font Files (*.ttf;*.otf;*.ttc)\0*.ttf;*.otf;*.ttc\0All Files (*.*)\0*.*\0";
        ofn.lpstrFile = buf;
        ofn.nMaxFile = (DWORD)(sizeof(buf)/sizeof(buf[0]));
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
        ofn.lpstrTitle = L"Open Font File";
        // Set initial file from CustomPath if exists
        if (CustomPath[0])
        {
            std::wstring wpath;
            int wlen = MultiByteToWideChar(CP_UTF8, 0, CustomPath, -1, nullptr, 0);
            wpath.resize(wlen);
            MultiByteToWideChar(CP_UTF8, 0, CustomPath, -1, wpath.data(), wlen);
            wcsncpy_s(buf, wpath.c_str(), _TRUNCATE);
            // ofn.lpstrInitialDir could be set to directory of path, but use file as initial
        }
        if (GetOpenFileNameW(&ofn))
        {
            out = WideToUtf8(buf);
            return true;
        }
#endif
        return false;
    }

    bool LoadCustomFont(const char* path)
    {
        if (!path || !*path) { LoadError = "Empty path"; return false; }
        LoadError.clear();

        // Wide range to support any TTF's alphabet (0x0020-0xFFFF). ImGui will skip missing glyphs via IsGlyphInFont, so cost is just a loop.
        // For very large CJK fonts this is still okay (~65k checks). If too heavy, fallback to Thai+Default.
        static const ImWchar kAllRanges[] = { 0x0020, 0xFFFF, 0 };
        ImFontConfig cfg;
        cfg.FontDataOwnedByAtlas = true;
        // keep oversampling default, MergeMode false -> standalone preview font
        ImGuiIO& io = ImGui::GetIO();
        ImFont* f = io.Fonts->AddFontFromFileTTF(path, 16.0f, &cfg, kAllRanges);
        if (!f)
        {
            // fallback to Thai+Default range (lighter)
            f = io.Fonts->AddFontFromFileTTF(path, 16.0f, &cfg, io.Fonts->GetGlyphRangesThai());
        }
        if (!f)
        {
            LoadError = std::string("Failed to load: ") + path + " (check path / file is valid TTF)";
            return false;
        }
        CustomFont = f;
        snprintf(CustomPath, sizeof(CustomPath), "%s", path);
        return true;
    }

    static std::string CodepointToUTF8(unsigned int cp)
    {
        std::string out;
        if (cp <= 0x7F) out.push_back((char)cp);
        else if (cp <= 0x7FF) { out.push_back((char)(0xC0 | (cp >> 6))); out.push_back((char)(0x80 | (cp & 0x3F))); }
        else if (cp <= 0xFFFF) { out.push_back((char)(0xE0 | (cp >> 12))); out.push_back((char)(0x80 | ((cp >> 6) & 0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
        else { out.push_back((char)(0xF0 | (cp >> 18))); out.push_back((char)(0x80 | ((cp >> 12) & 0x3F))); out.push_back((char)(0x80 | ((cp >> 6) & 0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
        return out;
    }

    void Draw()
    {
        if (!Show) return;

        ImGui::SetNextWindowSize(ImVec2(860, 700), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Language Test", &Show))
        {
            ImGui::End();
            return;
        }

        const float scale = FontSize / 16.0f;
        ImGui::SetWindowFontScale(scale);

        ImFont* font = CustomFont ? CustomFont : (ThaiFont ? ThaiFont : MainFont);
        if (!font) font = ImGui::GetFont();

        // ---- Font file picker (any .ttf on device) ----
        ImGui::SeparatorText("Font file");
        {
            ImGui::SetNextItemWidth(-140);
            ImGui::InputTextWithHint("##ttfpath", "C:\\Windows\\Fonts\\...\\.ttf or RD CHULAJARUEK.ttf", CustomPath, sizeof(CustomPath));
            ImGui::SameLine();
            if (ImGui::Button("Browse...")) {
                std::string picked;
                if (PickTtfFile(picked)) {
                    snprintf(CustomPath, sizeof(CustomPath), "%s", picked.c_str());
                    LoadCustomFont(CustomPath);
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Load")) {
                LoadCustomFont(CustomPath);
            }

            if (CustomFont && CustomFont->IsLoaded())
                ImGui::TextColored(ImVec4(0.5f,0.9f,0.5f,1), "Loaded: %s", CustomPath);
            else if (!LoadError.empty())
                ImGui::TextColored(ImVec4(1,0.4f,0.4f,1), "%s", LoadError.c_str());
            else
                ImGui::TextDisabled("Tip: Browse to C:\\Windows\\Fonts\\*.ttf or any .ttf on disk, then Load. Thai example: RD CHULAJARUEK.ttf");

            ImGui::TextDisabled("Atlas %dx%d  Fonts %d  |  Current: %s", ImGui::GetIO().Fonts->TexData->Width, ImGui::GetIO().Fonts->TexData->Height, (int)ImGui::GetIO().Fonts->Fonts.Size, font ? font->GetDebugName() : "null");
            // Quick system font shortcuts
            ImGui::TextDisabled("Quick open:");
            ImGui::SameLine();
            if (ImGui::SmallButton("Windows/Fonts folder")) {
                std::string p; if (PickTtfFile(p)) { snprintf(CustomPath, sizeof(CustomPath), "%s", p.c_str()); LoadCustomFont(CustomPath); }
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("RD CHULAJARUEK")) {
                snprintf(CustomPath, sizeof(CustomPath), "%s", "RD CHULAJARUEK.ttf");
                if (!LoadCustomFont(CustomPath)) LoadCustomFont("ROSE-image-editor/RD CHULAJARUEK.ttf");
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("ZF#2ndPixelus")) {
                snprintf(CustomPath, sizeof(CustomPath), "%s", "ZF#2ndPixelus.ttf");
                LoadCustomFont(CustomPath);
            }
        }

        ImGui::Spacing();

        // ---- Preview size (window-local) ----
        ImGui::SeparatorText("Preview");
        {
            ImGui::SetNextItemWidth(260);
            ImGui::SliderFloat("Size", &FontSize, 8.0f, 48.0f, "%.0f px");
            ImGui::SameLine(); if (ImGui::SmallButton("Reset")) FontSize = 16.0f;
            ImGui::SameLine(); ImGui::TextDisabled("(preview only)");
            ImGui::PushFont(font);
            ImGui::TextWrapped("%s", (const char*)u8"กขค  สวัสดี  Hello  123  — preview of loaded TTF");
            ImGui::PopFont();
        }

        // ---- Typing test ----
        ImGui::SeparatorText("Typing test");
        {
            static char bufSingle[512] = "";
            static char bufMulti[1024] = "";
            static bool s_init = false;
            if (!s_init)
            {
                s_init = true;
                snprintf(bufSingle, sizeof(bufSingle), "%s", (const char*)u8"สวัสดีครับ Hello 123");
                snprintf(bufMulti,  sizeof(bufMulti),  "%s", (const char*)u8"พิมพ์ภาษาไทยได้ที่นี่...\nHello World 12345\nกขฃคฅฆงจฉชซฌญฎฏฐฑฒณดตถทธนบปผฝพฟภมยรลวศษสหฬอฮ");
            }

            ImGui::PushFont(font);
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextWithHint("##single", (const char*)u8"พิมพ์ไทย / English... (uses loaded TTF)", bufSingle, sizeof(bufSingle));
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextMultiline("##multi", bufMulti, sizeof(bufMulti), ImVec2(-1, 78));
            ImGui::PopFont();

            ImGui::Spacing();
            ImGui::TextDisabled("Preview");
            ImGui::BeginChild("PreviewBox", ImVec2(-1, 64), true);
            ImGui::PushFont(font);
            ImGui::TextWrapped("%s", bufSingle);
            ImGui::TextWrapped("%s", bufMulti);
            ImGui::PopFont();
            ImGui::EndChild();
        }

        // ---- Glyph inventory (tabbed) ----
        ImGui::SeparatorText("Alphabet in loaded TTF");
        ImGui::TextDisabled("Green = glyph exists in this file, red = missing. Grids are 8 columns. Change file above to inspect any .ttf.");

        auto DrawBlock = [&](const char* label, unsigned int from, unsigned int to)
        {
            if (ImGui::BeginTable(label, 8, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_RowBg))
            {
                for (unsigned int cp = from; cp <= to; ++cp)
                {
                    ImGui::TableNextColumn();
                    std::string utf8 = CodepointToUTF8(cp);
                    bool exists = font->IsGlyphInFont((ImWchar)cp);
                    ImGui::PushFont(font);
                    if (exists) ImGui::TextColored(ImVec4(0.45f,0.95f,0.45f,1), "%s", utf8.c_str());
                    else        ImGui::TextDisabled("%s", utf8.c_str());
                    ImGui::PopFont();
                    if (exists) ImGui::TextDisabled("U+%04X", cp);
                    else        ImGui::TextColored(ImVec4(1,0.45f,0.45f,1), "U+%04X", cp);
                }
                ImGui::EndTable();
            }
        };

        if (ImGui::BeginTabBar("AlphabetTabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Consonants"))
            {
                DrawBlock("Consonants  ก-ฮ  (0x0E01-0x0E2E)", 0x0E01, 0x0E2E);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Vowels / Tones"))
            {
                DrawBlock("Vowels  (0x0E30-0x0E3A)", 0x0E30, 0x0E3A);
                DrawBlock("Tone & Signs  (0x0E40-0x0E4E)", 0x0E40, 0x0E4E);
                DrawBlock("Thai digits  (0x0E50-0x0E59)", 0x0E50, 0x0E59);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Full Thai"))
            {
                ImGui::BeginChild("FullThaiScroll", ImVec2(-1, 260), false);
                DrawBlock("Full block  (0x0E00-0x0E7F)", 0x0E00, 0x0E7F);
                ImGui::EndChild();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Latin"))
            {
                DrawBlock("A-Z  (0x41-0x5A)", 0x41, 0x5A);
                DrawBlock("a-z  (0x61-0x7A)", 0x61, 0x7A);
                DrawBlock("0-9  (0x30-0x39)", 0x30, 0x39);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Pangram"))
            {
                ImGui::PushFont(font);
                ImGui::Spacing();
                ImGui::TextWrapped("%s", (const char*)u8"ก ข ฃ ค ฅ ฆ ง จ ฉ ช ซ ฌ ญ ฎ ฏ ฐ ฑ ฒ ณ ด ต ถ ท ธ น บ ป ผ ฝ พ ฟ ภ ม ย ร ล ว ศ ษ ส ห ฬ อ ฮ");
                ImGui::Spacing();
                ImGui::TextWrapped("%s", (const char*)u8"สระ ะ า ิ ี ึ ื ุ ู เ แ โ ใ ไ ำ ็ ่ ้ ๊ ๋ ์ ํ ๏");
                ImGui::Spacing();
                ImGui::TextWrapped("%s", (const char*)u8"๑ ๒ ๓ ๔ ๕ ๖ ๗ ๘ ๙ ๐");
                ImGui::Spacing();
                ImGui::TextWrapped("%s", (const char*)u8"The quick brown fox jumps over the lazy dog 0123456789");
                ImGui::PopFont();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}
