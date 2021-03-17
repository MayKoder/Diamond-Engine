#include "IM_MaterialImporter.h"
#include"Application.h"
#include"DEJsonSupport.h"

#include "IM_FileSystem.h"
#include"MO_Scene.h"

#include "RE_Material.h"
#include "RE_Shader.h"

#include "Globals.h"

void MaterialImporter::CreateBaseMaterialFile(const char* path)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	std::string name;
	FileSystem::GetFileName(path, name, false);
	root_object.WriteString("name", name.c_str());
	root_object.WriteInt("ShaderUID", EngineExternal->moduleScene->defaultMaterial->shader->GetUID());

	JSON_Value* uniformsArray = json_value_init_array();
	EngineExternal->moduleScene->defaultMaterial->SaveToJson(json_value_get_array(uniformsArray));

	json_object_set_value(root_object.nObj, "Uniforms", uniformsArray);

	//Default shader does not have a library path
	if (json_serialize_to_file_pretty(file, path) != JSONFailure) 
	{
		LOG(LogType::L_NORMAL, "File saved at: %s", path);
	}
	else
	{
		LOG(LogType::L_ERROR, "Error trying to save at: %s", path);
	}
	
	json_value_free(file);
}

void MaterialImporter::Save(ResourceMaterial* material, char** fileBuffer)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	root_object.WriteString("name", material->name);
	root_object.WriteInt("ShaderUID", material->shader->GetUID());

	JSON_Value* uniformsArray = json_value_init_array();
	material->SaveToJson(json_value_get_array(uniformsArray));
	json_object_set_value(root_object.nObj, "Uniforms", uniformsArray);

	//TODO: Should be saving Assets material and not library but there is no assets path in the resource :(
	//Default shader does not have a library path
	//json_serialize_to_file_pretty(file, material->GetAssetPath());
	std::string assets_path = "Assets/Materials/" + std::string(material->name) + ".mat";
	json_serialize_to_file_pretty(file, assets_path.c_str());
	json_serialize_to_file_pretty(file, material->GetLibraryPath());

	json_value_free(file);

	//LOG(LogType::L_NORMAL, "File saved at: %s", material->GetAssetPath());
	LOG(LogType::L_NORMAL, "File saved at: %s", assets_path.c_str());
}

//void MaterialImporter::Save(uint uid, const char* path)
//{
	//Display material data
	//Case of shader switch
		//Remove from "references" vector from shader?
		//Save and import new material data?

//	JSON_Value* file = json_value_init_object();
//	DEConfig root_object(json_value_get_object(file));
//
//	root_object.WriteInt("ShaderUID", uid);
//
//	JSON_Value* uniformsArray = json_value_init_array();
//	material->SaveToJson(json_value_get_array(uniformsArray));
//	json_object_set_value(root_object.nObj, "Uniforms", uniformsArray);
//
//	//Default shader does not have a library path
//	//json_serialize_to_file_pretty(file, material->GetAssetPath());
//	json_serialize_to_file_pretty(file, material->GetLibraryPath());
//
//	json_value_free(file);
//
//	//LOG(LogType::L_NORMAL, "File saved at: %s", material->GetAssetPath());
//	LOG(LogType::L_NORMAL, "File saved at: %s", material->GetLibraryPath());
//}
