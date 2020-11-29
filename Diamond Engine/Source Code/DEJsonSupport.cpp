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
	json_object_set_string(obj, name, value);
}
const char* DEJson::ReadString(JSON_Object* obj, const char* name)
{
	return json_object_get_string(obj, name);
}
void DEJson::WriteBool(JSON_Object* obj, const char* name, bool value)
{
	json_object_set_boolean(obj, name, value);
}
bool DEJson::ReadBool(JSON_Object* obj, const char* name)
{
	return json_object_get_boolean(obj, name);
}
void DEJson::WriteInt(JSON_Object* obj, const char* name, int value)
{
	json_object_set_number(obj, name, value);
}
int DEJson::ReadInt(JSON_Object* obj, const char* name)
{
	return json_object_get_number(obj, name);
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

DEConfig::DEConfig(JSON_Object* _nObj)
{
	nObj = _nObj;
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
	return json_object_get_string(nObj, name);
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

void DEConfig::WriteVector3(const char* name, float* value)
{
	JSON_Value* goArray = json_value_init_array();
	PopulateArray(goArray, value, 3);
	json_object_set_value(nObj, name, goArray);
}
float3 DEConfig::ReadVector3(const char* name)
{
	JSON_Array* vecArray = json_object_dotget_array(nObj, name);
	return float3(json_array_get_number(vecArray, 0), json_array_get_number(vecArray, 1), json_array_get_number(vecArray, 2));
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
