#include "IM_MaterialImporter.h"
#include"Application.h"
#include"DEJsonSupport.h"

#include"MO_Scene.h"

#include "RE_Material.h"
#include "RE_Shader.h"

#include "Globals.h"

void MaterialImporter::CreateBaseMaterialFile(const char* path)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	root_object.WriteInt("ShaderUID", EngineExternal->moduleScene->defaultMaterial->shader->GetUID());

	JSON_Value* uniformsArray = json_value_init_array();
	EngineExternal->moduleScene->defaultMaterial->SaveToJson(json_value_get_array(uniformsArray));
	json_object_set_value(root_object.nObj, "Uniforms", uniformsArray);

	//Default shader does not have a library path
	//json_serialize_to_file_pretty(file, material->GetAssetPath());
	json_serialize_to_file_pretty(file, path);

	json_value_free(file);

	//LOG(LogType::L_NORMAL, "File saved at: %s", material->GetAssetPath());
	LOG(LogType::L_NORMAL, "File saved at: %s", path);
}

void MaterialImporter::Save(ResourceMaterial* material, char** fileBuffer)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	root_object.WriteInt("ShaderUID", material->shader->GetUID());

	JSON_Value* uniformsArray = json_value_init_array();
	material->SaveToJson(json_value_get_array(uniformsArray));
	json_object_set_value(root_object.nObj, "Uniforms", uniformsArray);
  
	//Default shader does not have a library path
	//json_serialize_to_file_pretty(file, material->GetAssetPath());
	json_serialize_to_file_pretty(file, material->GetLibraryPath());

	json_value_free(file);

	//LOG(LogType::L_NORMAL, "File saved at: %s", material->GetAssetPath());
	LOG(LogType::L_NORMAL, "File saved at: %s", material->GetLibraryPath());
}
