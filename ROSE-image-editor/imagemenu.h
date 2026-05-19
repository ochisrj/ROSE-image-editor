#pragma once

#include "imgui.h"
#include <iostream>

class ImageMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool Adjustment;
	static bool CanvasSize;
	static bool ImageSize;
	static bool RorateCanvas;
	static bool FlipHorizontal;
	static bool FlipVertical;
};