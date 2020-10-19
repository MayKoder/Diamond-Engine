#include"W_Assets.h"

W_Assets::W_Assets()
{
	name = "Assets";
}

W_Assets::~W_Assets()
{
}

void W_Assets::Draw()
{
	ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/);
	//ImGui::Separator();

	ImGui::End();
}
