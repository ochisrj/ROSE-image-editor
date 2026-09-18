#pragma once
#include "imgui.h"
#include <string>

namespace LanguageTest
{
    extern bool     Show;
    extern ImFont*  MainFont;
    extern ImFont*  ThaiFont;
    extern ImFont*  CustomFont;
    extern float    FontSize;
    extern char     CustomPath[1024];
    extern std::string LoadError;

    void SetFonts(ImFont* mainFont, ImFont* thaiFont);
    bool LoadCustomFont(const char* path);
    void Draw();
}
