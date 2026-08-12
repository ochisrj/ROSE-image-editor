#include "menubar.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

void MenuBar::Draw(GLFWwindow* window)
{	
	// Main Menu Bar
	if (ImGui::BeginMainMenuBar())
	{	
		FileMenu::DrawMenu(window);
		EditMenu::DrawMenu();
		ViewMenu::DrawMenu();
		ImageMenu::DrawMenu();
		LayerMenu::DrawMenu();	
		SelectMenu::DrawMenu();
		FilterMenu::DrawMenu();
		WindowMenu::DrawMenu();
		HelpMenu::DrawMenu();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("| %.1f FPS", io.Framerate);
		ImGui::EndMainMenuBar();
	}

	// Menu Windows
	FileMenu::DrawWindow();
	EditMenu::DrawWindow();
	ViewMenu::DrawWindow();
	ImageMenu::DrawWindow();
	LayerMenu::DrawWindow();
	SelectMenu::DrawWindow();
	FilterMenu::DrawWindow();
	WindowMenu::DrawWindow();
	HelpMenu::DrawWindow();

}