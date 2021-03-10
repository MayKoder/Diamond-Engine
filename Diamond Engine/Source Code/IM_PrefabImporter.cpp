#include "IM_PrefabImporter.h"
#include "GameObject.h"
#include "DEJsonSupport.h"
#include "Application.h"
#include "MO_ResourceManager.h"
#include "IM_FileSystem.h"
#include "MO_Scene.h"
#include "CO_Transform.h"

#include <string>

void PrefabImporter::SavePrefab(const char* assets_path, GameObject* gameObject)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	JSON_Value* goArray = json_value_init_array();
	gameObject->SaveToJson(json_value_get_array(goArray));

	
	json_object_set_string(root_object.nObj, "assets_path", assets_path);
	json_object_set_value(root_object.nObj, "Game Objects", goArray);

	//Save file 
	json_serialize_to_file_pretty(file, assets_path);

	//Free memory
	json_value_free(file);
}

GameObject* PrefabImporter::LoadPrefab(const char* libraryPath)
{
	GameObject* rootObject = nullptr;

	JSON_Value* prefab = json_parse_file(libraryPath);
	
	if (prefab == nullptr)
		return nullptr;

	JSON_Object* prefabObj = json_value_get_object(prefab);
	JSON_Array* gameObjects = json_object_get_array(prefabObj, "Game Objects");
	JSON_Object* goJsonObj = json_array_get_object(gameObjects, 0);

	rootObject = new GameObject(json_object_get_string(goJsonObj, "name"), EngineExternal->moduleScene->root, json_object_get_number(goJsonObj, "UID"));
	rootObject->LoadFromJson(goJsonObj);

	GameObject* parent = rootObject;

	for (size_t i = 1; i < json_array_get_count(gameObjects); i++)
	{
		parent = LoadGOData(json_array_get_object(gameObjects, i), parent);
	}

	rootObject->RecursiveUIDRegeneration();
	
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
	JSON_Object* prefabObj = json_value_get_object(prefab);
	const char* assets_path = json_object_get_string(prefabObj, "assets_path");

	SavePrefab(assets_path, gameObject);
	EngineExternal->moduleResources->ImportFile(assets_path, Resource::Type::PREFAB);

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
