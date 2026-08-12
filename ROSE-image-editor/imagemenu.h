#pragma once

#include "imgui.h"
#include <iostream>

class ImageMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
	static bool show_image_viewer;
private:
	static bool Adjustment;
	static bool CanvasSize;
	static bool ImageSize;
	static bool RorateCanvas;
	static bool FlipHorizontal;
	static bool FlipVertical;
};