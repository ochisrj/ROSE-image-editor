#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "menubar.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "implot.h"
#include "implot_internal.h"

#include "implot3d.h"
#include "implot3d_internal.h"

#include <iostream>

void MenuBar::Draw(GLFWwindow* window)
{	

	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGuiWindowFlags window_flags = 
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("Full Window", nullptr, window_flags);
	ImGui::End();

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
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("| %.1f FPS", io.Framerate);
		ImGui::EndMainMenuBar();
	}
	
	FileMenu::DrawWindow();
	EditMenu::DrawWindow();
	ViewMenu::DrawWindow();
	ImageMenu::DrawWindow();
	LayerMenu::DrawWindow();
	SelectMenu::DrawWindow();
	FilterMenu::DrawWindow();
	WindowMenu::DrawWindow();
}