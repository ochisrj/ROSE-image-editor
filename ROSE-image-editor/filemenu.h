#pragma once
#include <iostream>
struct GLFWwindow;

class FileMenu
{
public:
    static void DrawMenu(GLFWwindow* window);
    static void DrawWindow();
private:
    static bool NewFile;
    static bool OpenFile;
    static bool SaveFile;
    static bool SaveAsFile;
    static bool ExportFile;
    static bool Preference;
    static bool Exit;

};