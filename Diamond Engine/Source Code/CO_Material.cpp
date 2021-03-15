#include "CO_Material.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include"MO_Renderer3D.h"
#include"MO_Scene.h"

#include"RE_Texture.h"
#include"RE_Shader.h"
#include"MO_ResourceManager.h"

#include"DEJsonSupport.h"
#include"IM_TextureImporter.h"
#include"RE_Material.h"

#include "IM_FileSystem.h"
#include "IM_MaterialImporter.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), viewWithCheckers(false), matTexture(nullptr),
material(nullptr)
{
	name = "Material";
	material = (EngineExternal->moduleScene->defaultMaterial != nullptr) ? dynamic_cast<ResourceMaterial*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleScene->defaultMaterial->GetUID())) : NULL;
}

C_Material::~C_Material()
{
	if(matTexture != nullptr)
		EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());

	if (material != nullptr)
		EngineExternal->moduleResources->UnloadResource(material->GetUID());
}

#ifndef STANDALONE
bool C_Material::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		//if (matTexture && matTexture->textureID != 0) {


		//	//ImGui::Text("Name: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->name.c_str());
		//	ImGui::Text("Texture Width: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tWidth);
		//	ImGui::Text("Texture Height: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tHeight);
		//	
		//	ImGui::Separator();

		//	ImGui::Text("Drop any texture here to change it:");
		//	ImGui::Image((ImTextureID)matTexture->textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
		//	if (ImGui::BeginDragDropTarget())
		//	{
		//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
		//		{
		//			//Drop asset from Asset window to scene window
		//			std::string* metaFileDrop = (std::string*)payload->Data;

		//			EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());
		//			std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

		//			if (matTexture)
		//				EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());
		//			
		//			matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
		//			LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
		//		}
		//		ImGui::EndDragDropTarget();
		//	}
		//	ImGui::Separator();

		//	//ImGui::Text("Asset Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->GetAssetPath());
		//	ImGui::Text("Library Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->GetLibraryPath());

		//	ImGui::Checkbox("View with checkers", &viewWithCheckers);
		//}
		//else
		//{
		//	//BUG: FBX LOADED HAVE TEXTURE BUT IT'S EMPTY, FIND A BETTER WAY TO IMPORT NO TEXTURE MESHES
		//	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "No texture loaded, using default checkers");
		//	ImGui::Image((ImTextureID)EngineExternal->moduleRenderer3D->checkersTexture, ImVec2(64, 64));
		//	if (ImGui::BeginDragDropTarget())
		//	{
		//		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
		//		{
		//			//Drop asset from Asset window to scene window
		//			std::string* metaFileDrop = (std::string*)payload->Data;
		//			std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

		//			if (matTexture)
		//				EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());

		//			matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
		//			LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
		//		}
		//		ImGui::EndDragDropTarget();
		//	}
		//}

		ImGui::Text("Current material: ");

		ImGui::Text("Drop here to change material");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL"))
			{
				std::string* assetsPath = (std::string*)payload->Data;

				if(material != nullptr)
					EngineExternal->moduleResources->UnloadResource(material->GetUID());

				material = dynamic_cast<ResourceMaterial*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath->c_str()));
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SameLine();
		if (material == nullptr)
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "No material");
		else
			material->DrawEditor();

		if (material && material->shader) 
		{
			ImGui::Dummy(ImVec2(0, 15));
			ImGui::Text("Using shader: %s", material->shader->GetAssetPath());
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
	if (material != nullptr) 
	{
		DEJson::WriteString(nObj, "MaterialAssetPath", material->GetAssetPath());
		DEJson::WriteString(nObj, "MaterialLibraryPath", material->GetLibraryPath());
		DEJson::WriteInt(nObj, "MaterialUID", material->GetUID());
	}
	//TODO: Call texture importer and load data
}

void C_Material::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	//if (nObj.ReadBool("IsEmpty") == true)
	//	return;


	int w, h;
	w = h = 0;
	std::string texPath = nObj.ReadString("AssetPath");
	std::string texName = nObj.ReadString("LibraryPath");

	if (texName != "") 
		matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("UID"), texName.c_str()));


	if (material != nullptr) 
	{
		EngineExternal->moduleResources->UnloadResource(material->GetUID());
		material = nullptr;
	}

	if(nObj.ReadInt("MaterialUID") != 0)
		material = dynamic_cast<ResourceMaterial*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("MaterialUID"), Resource::Type::MATERIAL));
}