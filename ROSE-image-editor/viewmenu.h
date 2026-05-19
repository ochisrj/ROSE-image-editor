#pragma once

#include "imgui.h"
#include <iostream>

class ViewMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
    static bool ZoomIn;
    static bool ZoomOut;
    static bool FitOnScreen;
    static bool Rulers;
    static bool Grid;
    static bool Snap;
};

