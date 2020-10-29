#include "C_Material.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include"ModuleRenderer3D.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), textureID(-1), tWidth(0), tHeight(0),
viewWithCheckers(false)
{

}

C_Material::~C_Material()
{
}

void C_Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{

		if (textureID != -1) {

			ImGui::Text("Texture Width: %i", tWidth);
			ImGui::Text("Texture Height: %i", tHeight);
			ImGui::Image((ImTextureID)textureID, ImVec2(128, 128));

			ImGui::Checkbox("View with checkers", &viewWithCheckers);


		}

	}

}

int C_Material::GetTextureID()
{
	return (viewWithCheckers == false) ? textureID : EngineExternal->renderer3D->checkersTexture;
}
