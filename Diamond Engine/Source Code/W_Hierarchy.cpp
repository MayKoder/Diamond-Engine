#include "W_Hierarchy.h"

W_Hierarchy::W_Hierarchy()
{
	name = "Hierarchy";
}

W_Hierarchy::~W_Hierarchy()
{
}

void W_Hierarchy::Draw()
{

	ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*/);

	ImGui::End();

}
