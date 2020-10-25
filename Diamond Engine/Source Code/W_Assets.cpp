#include"W_Assets.h"

W_Assets::W_Assets() : Window()
{
	name = "Assets";
}

W_Assets::~W_Assets()
{
}

void W_Assets::Draw()
{
	if (ImGui::Begin(name.c_str(), NULL/*, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize*/)) 
	{
		//ImGui::Separator();
	}
	ImGui::End();
}
