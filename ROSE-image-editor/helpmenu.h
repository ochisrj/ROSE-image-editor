#pragma once

#include "imgui.h"
#include <iostream>

class HelpMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool AboutROSE;
	static bool KeyboardShortCut;

};