#pragma once

#include "imgui.h"
#include <iostream>

class WindowMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool LayerPanel;
	static bool Channels;
	static bool History;
	static bool Toolbar;
	static bool Performace;
	static bool ResetWorkspace;
	static bool PlotTwoD;
};