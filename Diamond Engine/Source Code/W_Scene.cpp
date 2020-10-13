#include "W_Scene.h"

W_Scene::W_Scene(Application* _app) /*: texColorBuffer(-1)*/
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
	ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*/,ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

	ImVec2 screenOffset = ImVec2(App->window->s_width, App->window->s_height);
	screenOffset.x /= 2;
	screenOffset.y /= 2;

	screenOffset.x -= ImGui::GetWindowSize().x / 2;
	screenOffset.y -= ImGui::GetWindowSize().y / 2;

	screenOffset.x *= -1;
	screenOffset.y *= -1;
	//

	ImGui::SetCursorPos(screenOffset);
	ImGui::Image((ImTextureID)App->renderer3D->texColorBuffer, ImVec2(App->window->s_width, App->window->s_height), ImVec2(0, 1), ImVec2(1, 0));

	ImGui::End();
	ImGui::PopStyleVar();

}
