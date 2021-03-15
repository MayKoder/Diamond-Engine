#include "IM_PrefabImporter.h"
#include "GameObject.h"
#include "DEJsonSupport.h"
#include "Application.h"
#include "MO_ResourceManager.h"
#include "IM_FileSystem.h"
#include "MO_Scene.h"
#include "CO_Transform.h"

#include <string>

int PrefabImporter::SavePrefab(const char* assets_path, GameObject* gameObject)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	JSON_Value* goArray = json_value_init_array();
	gameObject->SaveToJson(json_value_get_array(goArray));

	json_object_set_string(root_object.nObj, "assets_path", assets_path);
	json_object_set_value(root_object.nObj, "Game Objects", goArray);

	//Save file 
	json_serialize_to_file_pretty(file, assets_path);

	int uid = EngineExternal->moduleResources->ImportFile(assets_path, Resource::Type::PREFAB);
	EngineExternal->moduleResources->GenerateMeta(assets_path, EngineExternal->moduleResources->GenLibraryPath(uid, Resource::Type::PREFAB).c_str(),
												  uid, Resource::Type::PREFAB);

	//std::string meta = std::string(assets_path) + ".meta";
	//int uid = EngineExternal->moduleResources->GetMetaUID(meta.c_str());

	//Free memory
	json_value_free(file);

	return uid;
}

GameObject* PrefabImporter::LoadPrefab(const char* libraryPath)
{
	GameObject* rootObject = nullptr;

	JSON_Value* prefab = json_parse_file(libraryPath);
	
	if (prefab == nullptr)
		return nullptr;

	JSON_Object* prefabObj = json_value_get_object(prefab);
	JSON_Array* gameObjectsArray = json_object_get_array(prefabObj, "Game Objects");
	JSON_Object* goJsonObj = json_array_get_object(gameObjectsArray, 0);

	rootObject = new GameObject(json_object_get_string(goJsonObj, "name"), EngineExternal->moduleScene->root, json_object_get_number(goJsonObj, "UID"));
	rootObject->LoadFromJson(goJsonObj);

	GameObject* parent = rootObject;

	for (size_t i = 1; i < json_array_get_count(gameObjectsArray); i++)
	{
		parent = LoadGOData(json_array_get_object(gameObjectsArray, i), parent);
	}

	//rootObject->RecursiveUIDRegeneration();

	//Save all references to game objects with their old UID
	std::map<uint, GameObject*> gameObjects;
	rootObject->RecursiveUIDRegenerationSavingReferences(gameObjects);

	//replace the components references with the new GameObjects using their old UIDs
	std::map<uint, GameObject*>::const_iterator it = gameObjects.begin();
	for(it; it != gameObjects.end(); it++)
	{
		for (size_t i = 0; i < it->second->components.size(); i++)
		{
			it->second->components[i]->OnRecursiveUIDChange(gameObjects);
		}
	}
	
	std::string id_string;
	FileSystem::GetFileName(libraryPath, id_string, false);
	rootObject->prefabID = (uint)atoi(id_string.c_str());

	//Free memory
	json_value_free(prefab);

	return rootObject;
}

GameObject* PrefabImporter::LoadGOData(JSON_Object* goJsonObj, GameObject* parent)
{
	GameObject* originalParent = parent;

	while (parent != nullptr && json_object_get_number(goJsonObj, "ParentUID") != parent->UID)
		parent = parent->parent;

	if (parent == nullptr)
		parent = originalParent;

	parent = new GameObject(json_object_get_string(goJsonObj, "name"), parent, json_object_get_number(goJsonObj, "UID"));
	parent->LoadFromJson(goJsonObj);
	return parent;
}

void PrefabImporter::OverridePrefabGameObjects(uint prefabID, GameObject* gameObject)
{
	//SavePrefab();
	std::string libraryPath = EngineExternal->moduleResources->GenLibraryPath(prefabID, Resource::Type::PREFAB);

	JSON_Value* prefab = json_parse_file(libraryPath.c_str());
	std::string assets_path;

	if (prefab == nullptr)
	{
		if (!FileSystem::Exists(libraryPath.c_str())) {
			assets_path = "Assets/Prefabs/" + gameObject->name + ".prefab";
			LOG(LogType::L_ERROR, "The prefab tried to override does not exist, it will be created at: %s", assets_path.c_str());
		}
	}
	else
	{
		JSON_Object* prefabObj = json_value_get_object(prefab);
		assets_path = json_object_get_string(prefabObj, "assets_path");
	}

	SavePrefab(assets_path.c_str(), gameObject);
	//EngineExternal->moduleResources->ImportFile(assets_path.c_str(), Resource::Type::PREFAB);

	json_value_free(prefab);

	std::vector<GameObject*> sceneObjects;
	EngineExternal->moduleScene->GetAllGameObjects(sceneObjects);

	std::vector<GameObject*> prefabObjects;
	for (size_t i = 0; i < sceneObjects.size(); i++)
	{
		if (sceneObjects[i]->prefabID == prefabID)
			prefabObjects.push_back(sceneObjects[i]);
	}

	for (size_t i = 0; i < prefabObjects.size(); i++)
	{
		if(prefabObjects[i] != gameObject)
			OverrideGameObject(prefabID, prefabObjects[i]);
	}
}

void PrefabImporter::OverrideGameObject(uint prefabID, GameObject* objectToReplace)
{
	std::string libraryPath = EngineExternal->moduleResources->GenLibraryPath(prefabID, Resource::Type::PREFAB);

	GameObject* prefabObject = LoadPrefab(libraryPath.c_str());

	if (prefabObject == nullptr) {
		return;
	}

	C_Transform* oldObjectTransform = objectToReplace->transform;
	prefabObject->transform->SetTransformMatrix(oldObjectTransform->position, oldObjectTransform->rotation, oldObjectTransform->localScale);

	prefabObject->ChangeParent(objectToReplace->parent);

	objectToReplace->Destroy();
}
