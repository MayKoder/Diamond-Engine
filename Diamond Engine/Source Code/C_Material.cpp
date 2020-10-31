#include "C_Material.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include"ModuleRenderer3D.h"
#include"Texture.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), viewWithCheckers(false), matTexture(nullptr)
{

}

C_Material::~C_Material()
{
}

void C_Material::OnEditor()
{
	if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
	{

		if (matTexture->textureID != -1) {

			ImGui::Text("Texture Width: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tWidth);
			ImGui::Text("Texture Height: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tHeight);
			ImGui::Image((ImTextureID)matTexture->textureID, ImVec2(128, 128));

			ImGui::Checkbox("View with checkers", &viewWithCheckers);


		}

	}

}

int C_Material::GetTextureID()
{
	return (viewWithCheckers == false) ? matTexture->textureID : EngineExternal->moduleRenderer3D->checkersTexture;
	//return matTexture->textureID;
}
