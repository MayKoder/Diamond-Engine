#include "W_Configuration.h"


W_Configuration::W_Configuration() : displayWindow(false)
{
	name = "Configuration";
}

W_Configuration::~W_Configuration()
{
}

void W_Configuration::Draw()
{
	ImGui::Begin(name.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
	
	if (ImGui::CollapsingHeader("Application"))
	{
		if (ImGui::Button("Display example")) 
		{
			displayWindow = !displayWindow;
		}
	}

	if (displayWindow) 
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::ShowDemoWindow();
		ImGui::PopStyleColor();
	}

	ImGui::End();
}



