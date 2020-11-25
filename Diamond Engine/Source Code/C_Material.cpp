#include "C_Material.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include"ModuleRenderer3D.h"

#include"Texture.h"
#include"DEJsonSupport.h"
#include"I_TextureImporter.h"
#include"I_FileSystem.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), viewWithCheckers(false), matTexture(nullptr)
{
	name = "Material";
}

C_Material::~C_Material()
{
}

bool C_Material::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (matTexture && matTexture->textureID != -1) {


			ImGui::Text("Name: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->name.c_str());
			ImGui::Text("Texture Width: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tWidth);
			ImGui::Text("Texture Height: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tHeight);
			ImGui::Image((ImTextureID)matTexture->textureID, ImVec2(128, 128));
			ImGui::Text("Texture Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->path.c_str());

			ImGui::Checkbox("View with checkers", &viewWithCheckers);


		}
		else
		{
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "No texture loaded, using default checkers");
			ImGui::Image((ImTextureID)EngineExternal->moduleRenderer3D->checkersTexture, ImVec2(128, 128));
		}
		return true;
	}
	return false;
}

int C_Material::GetTextureID()
{
	return (viewWithCheckers == false && matTexture) ? matTexture->textureID : EngineExternal->moduleRenderer3D->checkersTexture;
	//return matTexture->textureID;
}

void C_Material::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	DEJson::WriteString(nObj, "Path", matTexture->path.c_str());
	DEJson::WriteString(nObj, "Name", matTexture->name.c_str());
	//TODO: Call texture importer and load data
}

void C_Material::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);
	//There is no _mesh yet lol

	int w, h;
	w = h = 0;
	std::string texPath = json_object_get_string(nObj, "Path");
	std::string texName = json_object_get_string(nObj, "Name");

	if (texName == "" || texPath == "") {
		LOG(LogType::L_WARNING, "Empty");
		return;
	}

	char* buffer = nullptr;
	int size = FileSystem::LoadToBuffer(texPath.c_str(), &buffer);
	GLuint id = TextureImporter::CustomLoadImage(buffer, size, &w, &h);


	matTexture = new Texture(id, w, h, texName, texPath);


	EngineExternal->moduleRenderer3D->globalTextures.push_back(matTexture);
	RELEASE_ARRAY(buffer);
}