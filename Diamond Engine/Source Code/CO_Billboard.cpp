#include "CO_Billboard.h"
#include "ImGui/imgui.h"

C_Billboard::C_Billboard(GameObject* _gm):Component(_gm), currentAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED)
{
	name = "Billboard";
}

C_Billboard::~C_Billboard()
{
}

#ifndef STANDALONE
bool C_Billboard::OnEditor()
{
	if(Component::OnEditor() == false)
		return false;

	ImGui::Separator();

	std::string tempAlignment = "test";
	switch (currentAlignment) {
	case BILLBOARD_ALIGNMENT::AXIS_ALIGNED: tempAlignment = "Axis Aligned"; break;
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED: tempAlignment = "Screen Aligned"; break;
	case BILLBOARD_ALIGNMENT::WORLD_ALIGNED: tempAlignment = "World Aligned"; break;
	}
	ImGui::Text("Current Billboard: %s", tempAlignment.c_str());

	if (ImGui::BeginMenu("Change billboard")) {
		if (ImGui::MenuItem("Screen Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED);
		if (ImGui::MenuItem("World Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::WORLD_ALIGNED);
		if (ImGui::MenuItem("Axis Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::AXIS_ALIGNED);

		ImGui::EndMenu();
	}

	return true;
}
#endif // !STANDALONE

void C_Billboard::SetAlignment(BILLBOARD_ALIGNMENT new_alignment)
{
	currentAlignment = new_alignment;
}

void C_Billboard::Draw()
{
}

void C_Billboard::UseAlignment()
{
}

void C_Billboard::ScreenAlign()
{
}

void C_Billboard::WorldAlign()
{
}

void C_Billboard::AxisAlign()
{
}
