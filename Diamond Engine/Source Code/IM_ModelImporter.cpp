#include "IM_ModelImporter.h"
#include"GameObject.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include"MO_Scene.h"
#include"MO_ResourceManager.h"

#include"IM_FileSystem.h"
#include"IM_MeshLoader.h"

#include"RE_Mesh.h"
#include"RE_Texture.h"
#include"DEResource.h"

void ModelImporter::Import(char* buffer, int bSize, Resource* res)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, bSize, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	if (scene != nullptr && scene->HasMeshes())
	{
		std::vector<ResourceMesh*> meshesOnModelUIDs;
		std::vector<ResourceTexture*> texturesOnModelUIDs;

		//This should not be here
		if (scene->HasMaterials())
		{
			//Needs to be moved to another place
			std::string generalPath(res->GetAssetPath());
			generalPath = generalPath.substr(0, generalPath.find_last_of("/\\") + 1);
			for (size_t k = 0; k < scene->mNumMaterials; k++)
			{
				aiMaterial* material = scene->mMaterials[k];
				aiString str = material->GetName();
				uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

				if (numTextures > 0)
				{
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

					std::string textureFileName = FileSystem::NormalizePath(path.C_Str());
					size_t isLong = textureFileName.find_last_of("/");
					if (isLong != textureFileName.npos)
						textureFileName = textureFileName.substr(isLong + 1);

					std::string localPath = generalPath;
					localPath = localPath.substr(0, localPath.find_last_of('/') + 1);
					localPath += FileSystem::NormalizePath(path.C_Str());

					std::string libraryPath = EngineExternal->moduleResources->GetMetaPath(localPath.c_str());

					uint UID = EngineExternal->moduleResources->GetMetaUID(libraryPath.c_str());
					libraryPath = EngineExternal->moduleResources->LibraryFromMeta(libraryPath.c_str());

					ResourceTexture* texture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(UID, libraryPath.c_str()));

					texturesOnModelUIDs.push_back(texture);

					path.Clear();
				}
				else
				{
					//TODO: Temporal, think of a better way
					//TODO: Request resource?
					//ResourceTexture* meshTexture = new ResourceTexture(White);
					texturesOnModelUIDs.push_back(nullptr);
				}
			}
		}

		//Load all meshes into mesh vector
		if (scene->HasMeshes())
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				meshesOnModelUIDs.push_back(MeshLoader::LoadMesh(scene->mMeshes[i]));
			}
		}

		//Save custom format model
		GameObject* root = new GameObject("First model GO", nullptr);

		std::string name = "";
		FileSystem::GetFileName(res->GetAssetPath(), name, false);

		MeshLoader::NodeToGameObject(scene->mMeshes, texturesOnModelUIDs, meshesOnModelUIDs, scene->mRootNode, root, name.c_str());
		ModelImporter::SaveModelCustom(root->children[0], res->GetLibraryPath());
		delete root;

		meshesOnModelUIDs.clear();
		texturesOnModelUIDs.clear();

		aiReleaseImport(scene);

		//ModelImporter::LoadModelCustom(res->GetLibraryPath());
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene"/*, scene->name*/);
}

void ModelImporter::SaveModelCustom(GameObject* root, const char* nameWithExtension)
{
	JSON_Value* file = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(file);

	JSON_Value* goArray = json_value_init_array();
	root->SaveToJson(json_value_get_array(goArray));
	json_object_set_value(root_object, "Model Objects", goArray);

	//Save file 
	//std::string dir = MODELS_PATH;
	//dir += nameWithExtension;
	json_serialize_to_file_pretty(file, nameWithExtension);
	//dir.clear();

	//Free memory
	json_value_free(file);
}

void ModelImporter::LoadModelCustom(const char* nameWithExtension)
{
	JSON_Value* scene = json_parse_file(nameWithExtension);

	if (scene == NULL)
		return;

	JSON_Object* sceneObj = json_value_get_object(scene);
	JSON_Array* sceneGO = json_object_get_array(sceneObj, "Model Objects");

	JSON_Object* goJsonObj = json_array_get_object(sceneGO, 0);
	GameObject* parent = EngineExternal->moduleScene->root;

	for (size_t i = 0; i < json_array_get_count(sceneGO); i++)
	{
		goJsonObj = json_array_get_object(sceneGO, i);
		GameObject* originalParent = parent;

		while (parent != nullptr && json_object_get_number(goJsonObj, "ParentUID") != parent->UID)
			parent = parent->parent;

		if (parent == nullptr)
			parent = originalParent;

		parent = new GameObject(json_object_get_string(goJsonObj, "name"), parent, json_object_get_number(goJsonObj, "UID"));
		parent->LoadFromJson(goJsonObj);
	}
	
	//Free memory
	json_value_free(scene);
}
