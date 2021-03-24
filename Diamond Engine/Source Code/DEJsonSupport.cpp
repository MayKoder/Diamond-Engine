#include "DEJsonSupport.h"

//Internal use only
void DEJson::PopulateArray(JSON_Value* _array, float* value, unsigned int size)
{
	JSON_Array* jsArray = json_value_get_array(_array);
	for (unsigned int i = 0; i < size; i++)
	{
		json_array_append_number(jsArray, value[i]);
	}
}

void DEJson::WriteFloat(JSON_Object* obj, const char* name, float value)
{
	json_object_set_number(obj, name, value);
}
float DEJson::ReadFloat(JSON_Object* obj, const char* name)
{
	return json_object_get_number(obj, name);
}
void DEJson::WriteString(JSON_Object* obj, const char* name, const char* value)
{
	if (strcmp(value, "") == 0)
		value = "Empty";

	json_object_set_string(obj, name, value);
}
const char* DEJson::ReadString(JSON_Object* obj, const char* name)
{
	const char* ret = json_object_get_string(obj, name);
	return ret;
}
void DEJson::WriteBool(JSON_Object* obj, const char* name, bool value)
{
	json_object_set_boolean(obj, name, value);
}
bool DEJson::ReadBool(JSON_Object* obj, const char* name)
{
	if (json_object_has_value(obj, name) == 1)
		return json_object_get_boolean(obj, name);
	else
		return false;
}
void DEJson::WriteInt(JSON_Object* obj, const char* name, int value)
{
	json_object_set_number(obj, name, value);
}
int DEJson::ReadInt(JSON_Object* obj, const char* name)
{
	return json_object_get_number(obj, name);
}
void DEJson::WriteVector2(JSON_Object* obj, const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 2);
	json_object_set_value(obj, name, goArray);
}

float2 DEJson::ReadVector2(JSON_Object* obj, const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(obj, name);
	return float2(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1));
}
void DEJson::WriteVector3(JSON_Object* obj, const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 3);
	json_object_set_value(obj, name, goArray);
}
float3 DEJson::ReadVector3(JSON_Object* obj, const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(obj, name);
	return float3(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1), json_array_get_number(vecArray, 2));
}
void DEJson::WriteVector4(JSON_Object* obj, const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 4);
	json_object_set_value(obj, name, goArray);
}

float4 DEJson::ReadVector4(JSON_Object* obj, const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(obj, name);
	return float4(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1), json_array_get_number(vecArray, 2), json_array_get_number(vecArray, 3));
}
void DEJson::WriteQuat(JSON_Object* obj, const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 4);
	json_object_set_value(obj, name, goArray);
}

//TODO: Make some safer code and remove duplicated code on readvector and readquat
Quat DEJson::ReadQuat(JSON_Object* obj, const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(obj, name);
	return Quat(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1), json_array_get_number(vecArray, 2), json_array_get_number(vecArray, 3));
}

DEConfig::DEConfig()
{
	root = json_value_init_object();
	nObj = json_value_get_object(root);
}

/*
DEConfig::DEConfig(const char* buffer) : root(nullptr), nObj(nullptr)
{
	root = json_parse_string(buffer);

	if (root != NULL)
	{
		nObj = json_value_get_object(root);
	}
	else
	{
		LOG(LogType::L_WARNING, "Could not parse json file",);
	}
}
*/

DEConfig::DEConfig(JSON_Object* _nObj) : root(nullptr)
{
	nObj = _nObj;
}

uint DEConfig::Save(char** buffer)
{
	uint size = json_serialization_size_pretty(root);
	*buffer = new char[size];
	json_serialize_to_buffer_pretty(root, *buffer, size);
	return size;
}

// ---------------------------------------------------------------------------------- //
//Internal use only
void DEConfig::PopulateArray(JSON_Value* _array, float* value, unsigned int size)
{
	JSON_Array* jsArray = json_value_get_array(_array);
	for (unsigned int i = 0; i < size; i++)
	{
		json_array_append_number(jsArray, value[i]);
	}
}


void DEConfig::WriteFloat(const char* name, float value)
{
	json_object_set_number(nObj, name, value);
}
float DEConfig::ReadFloat(const char* name)
{
	return json_object_get_number(nObj, name);
}
void DEConfig::WriteString(const char* name, const char* value)
{
	json_object_set_string(nObj, name, value);
}
const char* DEConfig::ReadString(const char* name)
{
	return (json_object_get_string(nObj, name) != nullptr) ? json_object_get_string(nObj, name) : "";
}
void DEConfig::WriteBool(const char* name, bool value)
{
	json_object_set_boolean(nObj, name, value);
}
bool DEConfig::ReadBool(const char* name)
{
	return json_object_get_boolean(nObj, name);
}
void DEConfig::WriteInt(const char* name, int value)
{
	json_object_set_number(nObj, name, value);
}

int DEConfig::ReadInt(const char* name)
{
	return json_object_get_number(nObj, name);
}

void DEConfig::WriteVector2(const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 2);
	json_object_set_value(nObj, name, goArray);
}

float2 DEConfig::ReadVector2(const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(nObj, name);

	if (vecArray == nullptr)
		return float2::one;

	return float2(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1));
}

void DEConfig::WriteVector3(const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 3);
	json_object_set_value(nObj, name, goArray);
}
float3 DEConfig::ReadVector3(const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(nObj, name);

	if (vecArray == nullptr)
		return float3::one;

	return float3(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1), json_array_get_number(vecArray, 2));
}
void DEConfig::WriteVector4(const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 4);
	json_object_set_value(nObj, name, goArray);
}

float4 DEConfig::ReadVector4(const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(nObj, name);

	if (vecArray == nullptr)
		return float4::one;

	return float4(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1), json_array_get_number(vecArray, 2), json_array_get_number(vecArray, 3));
}
void DEConfig::WriteQuat(const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 4);
	json_object_set_value(nObj, name, goArray);
}

//TODO: Make some safer code and remove duplicated code on readvector and readquat
Quat DEConfig::ReadQuat(const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(nObj, name);
	return Quat(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1), json_array_get_number(vecArray, 2), json_array_get_number(vecArray, 3));
}

JSON_Array* DEConfig::ReadArray(const char* name)
{
	return json_object_get_array(nObj, name);
}

JSON_Object* DEConfig::ReadObject(const char* name)
{
	return json_object_get_object(nObj, name);
}

JSON_Value* DEConfig::ReadValue(const char* name)
{
	return json_object_get_value(nObj, name);
}

DEConfigArray::DEConfigArray() : _array(nullptr)
{
	_value = json_value_init_array();
	json_value_get_array(_value);
}

void DEConfigArray::AddObject(DEConfig& object)
{
	json_array_append_value(_array, object.root);
}

int DEConfigArray::Size()
{
	return json_array_get_count(_array);
}
