#include "C_Material.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include"MO_Renderer3D.h"

#include"RE_Texture.h"
#include"MO_ResourceManager.h"

#include"DEJsonSupport.h"
#include"IM_TextureImporter.h"
#include"IM_FileSystem.h"

C_Material::C_Material(GameObject* _gm) : Component(_gm), viewWithCheckers(false), matTexture(nullptr)
{
	name = "Material";
}

C_Material::~C_Material()
{
	if(matTexture != nullptr)
		EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());
}

bool C_Material::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (matTexture && matTexture->textureID != 0) {


			//ImGui::Text("Name: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->name.c_str());
			ImGui::Text("Texture Width: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tWidth);
			ImGui::Text("Texture Height: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", matTexture->tHeight);
			
			ImGui::Image((ImTextureID)matTexture->textureID, ImVec2(128, 128));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					//Drop asset from Asset window to scene window
					std::string* metaFileDrop = (std::string*)payload->Data;

					//TODO: This is working only with textures now
					//EngineExternal->moduleResources->AssetsToScene(name);
					EngineExternal->moduleResources->UnloadResource(matTexture->GetUID());
					std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());
					
					matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
					LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::Text("Texture Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->GetAssetPath());
			ImGui::Text("Texture Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", matTexture->GetLibraryPath());

			ImGui::Checkbox("View with checkers", &viewWithCheckers);


		}
		else
		{
			//BUG: FBX LOADED HAVE TEXTURE BUT IT'S EMPTY, FIND A BETTER WAY TO IMPORT NO TEXTURE MESHES
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "No texture loaded, using default checkers");
			ImGui::Image((ImTextureID)EngineExternal->moduleRenderer3D->checkersTexture, ImVec2(128, 128));
		}
		return true;
	}
	return false;
}

int C_Material::GetTextureID()
{
	return (viewWithCheckers == false && (matTexture && matTexture->textureID != 0)) ? matTexture->textureID : EngineExternal->moduleRenderer3D->checkersTexture;
	//return matTexture->textureID;
}

void C_Material::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	if (matTexture) 
	{
		DEJson::WriteString(nObj, "AssetPath", matTexture->GetAssetPath());
		DEJson::WriteString(nObj, "LibraryPath", matTexture->GetLibraryPath());
		DEJson::WriteInt(nObj, "UID", matTexture->GetUID());
	}
	//TODO: Call texture importer and load data
}

void C_Material::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);
	//There is no _mesh yet lol

	int w, h;
	w = h = 0;
	std::string texPath = json_object_get_string(nObj, "AssetPath");
	std::string texName = json_object_get_string(nObj, "LibraryPath");

	if (texName == "" && texPath == "") {
		LOG(LogType::L_WARNING, "Empty");
		return;
	}

	DEConfig jsObj(nObj);

	matTexture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(jsObj.ReadInt("UID"), texName.c_str()));
	//matTexture->LoadToMemory();

	//char* buffer = nullptr;
	//int size = FileSystem::LoadToBuffer(texPath.c_str(), &buffer);
	//GLuint id = TextureImporter::CustomLoadImage(buffer, size, &w, &h);

	//TODO: Request resource
	//matTexture = new ResourceTexture(id, w, h, texName, texPath);


	//EngineExternal->moduleRenderer3D->globalTextures.push_back(matTexture);
	//RELEASE_ARRAY(buffer);
}