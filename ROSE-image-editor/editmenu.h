#pragma once

#include "imgui.h"
#include <iostream>

class EditMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool Undo;
	static bool Redo;
	static bool Cut;
	static bool Copy;
	static bool Paste;
	static bool Performace;
	static bool TriangleConfig;
};
