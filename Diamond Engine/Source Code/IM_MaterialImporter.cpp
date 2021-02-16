#include "IM_MaterialImporter.h"
#include "parson/parson.h"
#include"DEJsonSupport.h"
#include "RE_Material.h"

void MaterialImporter::Save(ResourceMaterial* material, char** fileBuffer)
{
	JSON_Value* file = json_value_init_object();
	DEConfig root_object(json_value_get_object(file));

	JSON_Value* uniformsArray = json_value_init_array();
	material->SaveToJson(json_value_get_array(uniformsArray));
	json_object_set_value(root_object.nObj, "Uniforms", uniformsArray);
}
