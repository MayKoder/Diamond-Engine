#include "IM_PrefabImporter.h"
#include "GameObject.h"
#include "DEJsonSupport.h"
#include "Application.h"
#include "MO_ResourceManager.h"
#include "IM_FileSystem.h"
#include "MO_Scene.h"

void PrefabImporter::SavePrefab(const char* folder, GameObject* gameObject)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	JSON_Value* goArray = json_value_init_array();
	gameObject->SaveToJson(json_value_get_array(goArray));

	std::string assets_path = std::string(folder) + "/" + gameObject->name + ".prefab";
	json_object_set_string(root_object.nObj, "assets_path", assets_path.c_str());
	json_object_set_value(root_object.nObj, "Game Objects", goArray);

	//Save file 
	json_serialize_to_file_pretty(file, assets_path.c_str());

	//std::string library_path = std::string(PREFABS_PATH) + std::to_string(EngineExternal->moduleResources->GenerateNewUID()) + ".prefab";
	//json_serialize_to_file_pretty(file, library_path.c_str());

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

	GameObject* parent = rootObject;

	for (size_t i = 1; i < json_array_get_count(gameObjects); i++)
	{
		parent = LoadGOData(json_array_get_object(gameObjects, i), parent);
	}

	rootObject->RecursiveUIDRegeneration();

	//Free memory
	json_value_free(prefab);
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
