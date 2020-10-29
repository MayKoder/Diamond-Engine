#include "W_Scene.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"

#include "OpenGL.h"
#include"Application.h"

W_Scene::W_Scene(Application* _app) : Window() /*: texColorBuffer(-1)*/
{
	name = "Scene";
	App = _app;
}

W_Scene::~W_Scene()
{

}

void W_Scene::InitSceneView()
{

}

void W_Scene::Draw()
{

	//ASK: Is rendering the whole image as 16:9 and hiding the overflow the worst idea ever?
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*/, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) 
	{
		if (/*ImGui::IsWindowFocused() &&*/ ImGui::IsWindowHovered()) 
		{
			App->moduleCamera->ProcessSceneKeyboard();
		}


		ImVec2 texOriginalSize = ImVec2(App->moduleWindow->s_width, App->moduleWindow->s_height);
		ImVec2 e = ImGui::GetWindowSize();

		ImVec2 startPoint = ImVec2((texOriginalSize.x / 2) - (e.x / 2), (texOriginalSize.y / 2) + (e.y / 2));
		ImVec2 endPoint = ImVec2((texOriginalSize.x / 2) + (e.x / 2), (texOriginalSize.y / 2) - (e.y / 2));


		// Normalized coordinates of pixel (10,10) in a 256x256 texture.
		ImVec2 uv0 = ImVec2(startPoint.x / texOriginalSize.x, startPoint.y / texOriginalSize.y);

		// Normalized coordinates of pixel (110,210) in a 256x256 texture.
		ImVec2 uv1 = ImVec2(endPoint.x / texOriginalSize.x, endPoint.y / texOriginalSize.y);

		// Display the 100x200 section starting at (10,10)
		ImGui::Image((ImTextureID)App->moduleRenderer3D->texColorBuffer, e, uv0, uv1);
	}
	ImGui::End();
	ImGui::PopStyleVar();

}
