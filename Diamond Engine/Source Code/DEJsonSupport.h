#pragma once

#include"parson/parson.h"

//DE means Diamond Engine :)
namespace DEJson 
{
	//Internal use only
	void PopulateArray(JSON_Value* _array, float* value, unsigned int size)
	{
		JSON_Array* jsArray = json_value_get_array(_array);
		for (unsigned int i = 0; i < size; i++)
		{
			json_array_append_number(jsArray, value[i]);
		}

	}


	void WriteFloat(JSON_Object* obj, const char* name, float value)
	{
		json_object_set_number(obj, name, value);
	}
	void WriteInt(JSON_Object* obj, const char* name, int value)
	{
		json_object_set_number(obj, name, value);
	}
	void WriteVector3(JSON_Object* obj, const char* name, float* value)
	{
		JSON_Value* goArray = json_value_init_array();
		PopulateArray(goArray, value, 3);
		json_object_set_value(obj, name, goArray);
	}
	void WriteQuat(JSON_Object* obj, const char* name, float* value)
	{
		JSON_Value* goArray = json_value_init_array();
		PopulateArray(goArray, value, 4);
		json_object_set_value(obj, name, goArray);
	}
}