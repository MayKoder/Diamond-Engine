#include "W_Configuration.h"


W_Configuration::W_Configuration()
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

	}


	ImGui::End();
}



