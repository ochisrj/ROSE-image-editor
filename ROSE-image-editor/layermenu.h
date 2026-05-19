#pragma once

#include "imgui.h"
#include <iostream>

class LayerMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool NewLayer;
	static bool DuplicateLayer;
	static bool DeleteLayer;
	static bool LayerStyle;
	static bool MergeDown;
	static bool FlattenImage;
};