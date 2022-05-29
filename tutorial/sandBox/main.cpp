
#include "igl/opengl/glfw/renderer.h"
#include "tutorial/sandBox/inputManager.h"
#include "sandBox.h"
#include <imgui/imgui.cpp>
#include <igl/get_seconds.h>
#include <external/glfw/include/GLFW/glfw3.h>
#include <Windows.h>
#include <MMSystem.h>

#pragma comment(lib, "winmm.lib")

using namespace std;
void ToggleMusicButton(const char* str_id, bool* v)
{
	static bool enable_7m = false;  // default value, the button is disabled 
	static float b = 1.0f; //  test whatever color you need from imgui_demo.cpp e.g.
	static float c = 0.5f; // 
	static int i = 3;
		if (enable_7m == true)
		{
			ImGui::PushID(" Music ");
			ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0, 1, 4));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0, 0, 0.5));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(i / 7.0f, 0, 1));
			ImGui::Button(" Music ");
			if (ImGui::IsItemClicked(0))
			{
				enable_7m = !enable_7m;
				PlaySound(TEXT("C:/Users/Almogi/Desktop/animation_course-master/tutorial/sandBox/backgroundMusic.wav"), NULL, SND_NODEFAULT | SND_LOOP | SND_ASYNC);	
			}
			ImGui::PopStyleColor(3);
			ImGui::PopID();
		}
		else
		{
			if (ImGui::Button(" Music ")) {
				enable_7m = true;
				PlaySound(NULL, 0, 0);
			}
				
		}
}

static bool toggleButton(const char* id, SandBox& viewer) {
	static float b = 5.0f; //  test whatever color you need from imgui_demo.cpp e.g.
	static float c = 5.0f; // 
	static int i = 3;
	static bool enable_7m = false;  // default value, the button is disabled 

	bool showWindow = true;
	if (enable_7m == true)
	{
		viewer.isActive = true;
		showWindow = false;
		enable_7m = false;
	}
	else if (viewer.isNextLevel) {
		if (ImGui::Button("\t\tStart Over\t\t")) {
			showWindow = true;
			enable_7m = true;
			viewer.isNextLevel = false;
			viewer.isActive = true;
			viewer.score = 0;
			viewer.level = 1;
		}else if (ImGui::Button("\t\tNext Level\t\t")) {
			showWindow = true;
			enable_7m = true;
			viewer.isNextLevel = false;
			viewer.isActive = true;
			viewer.level += 1;
		}
	}
	else if (viewer.isResume) {
		if (ImGui::Button("\t\tResume Game\t\t")) {
			showWindow = true;
			enable_7m = true;
			viewer.isActive = true;
			viewer.isResume = false;
			viewer.isGameStarted = true;
			viewer.isNextLevel = false;
		}
	}
	else
	{
		if (ImGui::Button("\t\tStart Game"))
			enable_7m = true;
	}
	return showWindow;
}


void TextCentered(std::string text, int info) {
	auto windowWidth = ImGui::GetWindowSize().x;
	auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str(), info);
}

//end project
int main(int argc, char* argv[])
{
	PlaySound(TEXT("C:/Users/Almogi/Desktop/animation_course-master/tutorial/sandBox/backgroundMusic.wav"), NULL, SND_NODEFAULT | SND_LOOP | SND_ASYNC);

	Display* disp = new Display(1600, 960, "Snake 3D");
	Renderer renderer;
	SandBox viewer;
	igl::opengl::glfw::imgui::ImGuiMenu menu;
	viewer.Init("C:/Users/Almogi/Desktop/animation_course-master/build/configuration.txt");
	//viewer.data_list[0].tree.init(viewer.data_list[0].V, viewer.data_list[0].F);
	//igl::AABB<Eigen::MatrixXd, 3> tree_first = viewer.data_list[0].tree;
	//Eigen::AlignedBox<double, 3> box_first = tree_first.m_box;
	
	//viewer.data_list[0].drawBox(box_first, 0);
	disp->SetRenderer(&renderer);



	menu.callback_draw_custom_window = [&]()
	{
		ImGui::CreateContext();
		ImGui::SetNextWindowPos(ImVec2(0.f * menu.menu_scaling(), 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(400, 1000), ImGuiCond_Always);
		static bool showWindow = true;
		static bool turnMusic = true;
		
		if (showWindow && viewer.level == 1) {
			viewer.isGameStarted = false;
			if (!ImGui::Begin(
				"Start Playing!", &showWindow,
				ImGuiWindowFlags_NoSavedSettings
			)) {
				ImGui::End();
			}
			else {
				ToggleMusicButton("music", &turnMusic);
				ImGui::SetWindowFontScale(1.5f);

				ImGui::PushItemWidth(-80);
				ImGui::Text("\n\n\n\n\n\n");
				TextCentered("Level: %d", viewer.level);
				TextCentered("Score: %d", viewer.score);
				ImGui::Text("\n\n");
				ImGui::PopItemWidth();
				showWindow = toggleButton("Start", viewer);
				ImGui::End();
			}
		}
		else if (viewer.isNextLevel)
		{
			viewer.isGameStarted = false;
			if (!ImGui::Begin(
				"Next Level", &showWindow,
				ImGuiWindowFlags_NoSavedSettings
			)) {
				ImGui::End();
			}
			else {
				ToggleMusicButton("music", &turnMusic);
				ImGui::SetWindowFontScale(1.5f);
				ImGui::PushItemWidth(-80);
				TextCentered("You Have Reached Score %d \n\tContinue Play?", viewer.score);
				ImGui::Text("\n\n");
				TextCentered("Level: %d", viewer.level);
				TextCentered("Score: %d", viewer.score);
				ImGui::Text("\n\n");
				ImGui::PopItemWidth();
				showWindow = toggleButton("START OVER", viewer);
				ImGui::End();
			}
		}
		else if (viewer.isResume) {
			viewer.isGameStarted = false;
			if (!ImGui::Begin(
				"Resume game When you are Ready To Play", &showWindow,
				ImGuiWindowFlags_NoSavedSettings
			)) {
				ImGui::End();
			}
			else {
				ToggleMusicButton("music", &turnMusic);
				ImGui::SetWindowFontScale(1.5f);
				
				ImGui::PushItemWidth(-80);
				ImGui::Text("\n\n\n\n");
				TextCentered("Level: %d", viewer.level);
				TextCentered("Score: %d", viewer.score);
				ImGui::Text("");
				ImGui::PopItemWidth();
				showWindow = toggleButton("Resume", viewer);
				ImGui::End();
			}
		}
		else {
			if (!ImGui::Begin(
				"Let's Play!", &showWindow,
				ImGuiWindowFlags_NoSavedSettings
			)) {
				ImGui::End();
			}
			else {
				ToggleMusicButton("music", &turnMusic);
				ImGui::SetWindowFontScale(1.5f);
				viewer.isGameStarted = true;
				ImGui::PushItemWidth(-80);
				ImGui::Text("\n\n\n\n");
				TextCentered("Level: %d", viewer.level);
				TextCentered("Score: %d", viewer.score);
				ImGui::PopItemWidth();
				ImGui::End();
			}

		}


	};

	Init(*disp, &menu);
	renderer.init(&viewer, 2, &menu); // 3
	renderer.selected_core_index = 1; // ?

	disp->launch_rendering(true);

	//delete menu;
	delete disp;
}