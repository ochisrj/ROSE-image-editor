#pragma once

#include "imgui.h"
#include <iostream>

class FileMenu 
{
public:
	static void DrawMenu();
	static void DrawWindow();
private:
	static bool NewFile;
	static bool OpenFile;
	static bool SaveFile;
	static bool SaveAsFile;
	static bool ExportFile;
	static bool Exit;
};