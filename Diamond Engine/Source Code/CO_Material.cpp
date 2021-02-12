#include "CO_Material.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include"MO_Renderer3D.h"

#include"RE_Texture.h"
#include"RE_Shader.h"
#include"MO_ResourceManager.h"
#include"MO_Scene.h"

#include"DEJsonSupport.h"
#include"IM_TextureImporter.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), viewWithCheckers(false), matTexture(nullptr),
shader(nullptr)
{
	name = "Material";
	shader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleScene->defaultShader->GetUID()));
}

C_Material::~C_Material()
{
	if(matTexture != nullptr)
		EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());

	if (shader != nullptr)
		EngineExternal->moduleResources->UnloadResource(shader->GetUID());
}

#ifndef STANDALONE
bool C_Material::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (matTexture && matTexture->textureID != 0) {


			//ImGui::Text("Name: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->name.c_str());
			ImGui::Text("Texture Width: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tWidth);
			ImGui::Text("Texture Height: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tHeight);
			
			ImGui::Separator();

			ImGui::Text("Drop any texture here to change it:");
			ImGui::Image((ImTextureID)matTexture->textureID, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					//Drop asset from Asset window to scene window
					std::string* metaFileDrop = (std::string*)payload->Data;

					EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());
					std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());
					
					matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
					LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::Separator();

			//ImGui::Text("Asset Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->GetAssetPath());
			ImGui::Text("Library Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->GetLibraryPath());

			ImGui::Checkbox("View with checkers", &viewWithCheckers);
		}
		else
		{
			//BUG: FBX LOADED HAVE TEXTURE BUT IT'S EMPTY, FIND A BETTER WAY TO IMPORT NO TEXTURE MESHES
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "No texture loaded, using default checkers");
			ImGui::Image((ImTextureID)EngineExternal->moduleRenderer3D->checkersTexture, ImVec2(128, 128));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					//Drop asset from Asset window to scene window
					std::string* metaFileDrop = (std::string*)payload->Data;
					std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

					matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
					LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::Text("Drop here to change shader");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SHADER"))
			{
				std::string* metaFileDrop = (std::string*)payload->Data;
				std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

				EngineExternal->moduleResources->UnloadResource(shader->GetUID());
				shader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
			}
			ImGui::EndDragDropTarget();
		}

		if (shader) 
		{
			ImGui::Dummy(ImVec2(0, 15));
			ImGui::Text("Using shader %s", shader->GetAssetPath());
			shader->DrawEditor();
		}
		return true;
	}
	return false;
}
#endif // !STANDALONE

int C_Material::GetTextureID()
{
	return (viewWithCheckers == false && (matTexture && matTexture->textureID != 0)) ? matTexture->textureID : EngineExternal->moduleRenderer3D->checkersTexture;
	//return matTexture->textureID;
}

void C_Material::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	DEJson::WriteBool(nObj, "IsEmpty", (matTexture == nullptr) ? true : false);
	if (matTexture != nullptr) 
	{
		DEJson::WriteString(nObj, "AssetPath", matTexture->GetAssetPath());
		DEJson::WriteString(nObj, "LibraryPath", matTexture->GetLibraryPath());
		DEJson::WriteInt(nObj, "UID", matTexture->GetUID());
	}
	if (shader != nullptr) 
	{
		DEJson::WriteString(nObj, "ShaderAssetPath", shader->GetAssetPath());
		DEJson::WriteString(nObj, "ShaderLibraryPath", shader->GetLibraryPath());
		DEJson::WriteInt(nObj, "ShaderUID", shader->GetUID());
	}
	//TODO: Call texture importer and load data
}

void C_Material::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	if (nObj.ReadBool("IsEmpty") == true)
		return;


	int w, h;
	w = h = 0;
	std::string texPath = nObj.ReadString("AssetPath");
	std::string texName = nObj.ReadString("LibraryPath");

	if (texName == "" && texPath == "") {
		LOG(LogType::L_WARNING, "Empty");
		return;
	}

	matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("UID"), texName.c_str()));

	if (shader != nullptr) 
	{
		EngineExternal->moduleResources->UnloadResource(shader->GetUID());
		shader = nullptr;
	}

	shader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("ShaderUID")));
}