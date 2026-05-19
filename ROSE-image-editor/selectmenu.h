#pragma once

#include "imgui.h"
#include <iostream>

class SelectMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool SelectAll;
	static bool Deselect;
	static bool Inverse;
	static bool ColorRange;
	static bool Modify;
};