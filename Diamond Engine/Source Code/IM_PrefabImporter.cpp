#include "IM_PrefabImporter.h"
#include "GameObject.h"
#include "DEJsonSupport.h"
#include "Application.h"
#include "MO_ResourceManager.h"
#include "IM_FileSystem.h"

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