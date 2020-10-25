#include "C_Material.h"
#include "ImGui/imgui.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), textureID(-1)
{
}

C_Material::~C_Material()
{
}

void C_Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{

		if(textureID != -1)
			ImGui::Image((ImTextureID)textureID, ImVec2(128, 128));

	}

}
