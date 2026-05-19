#pragma once

#include "imgui.h"
#include <iostream>

class FilterMenu
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool Blurs;
	static bool GaussianBlur;
	static bool MotionBlur;
	static bool BoxBlur;
	static bool Sharpen;
	static bool UnsharpedMask;
	static bool SmartSharpen;
	static bool Distort;
	static bool Liquify;
	static bool Wave;
	static bool Noise;
	static bool AddNoise;
	static bool Despeckle;
	static bool LastFilter;
};