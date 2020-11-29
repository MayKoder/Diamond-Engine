#pragma once

#include"parson/parson.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/Quat.h"

//DE means Diamond Engine :)
namespace DEJson 
{
	//Internal use only
	void PopulateArray(JSON_Value* _array, float* value, unsigned int size);


	void WriteFloat(JSON_Object* obj, const char* name, float value);
	float ReadFloat(JSON_Object* obj, const char* name);

	void WriteString(JSON_Object* obj, const char* name, const char* value);
	const char* ReadString(JSON_Object* obj, const char* name);

	void WriteBool(JSON_Object* obj, const char* name, bool value);
	bool ReadBool(JSON_Object* obj, const char* name);

	void WriteInt(JSON_Object* obj, const char* name, int value);
	int ReadInt(JSON_Object* obj, const char* name);

	void WriteVector3(JSON_Object* obj, const char* name, float* value);
	float3 ReadVector3(JSON_Object* obj, const char* name);

	void WriteQuat(JSON_Object* obj, const char* name, float* value);
	Quat ReadQuat(JSON_Object* obj, const char* name);
}

struct DEConfig
{

	DEConfig(JSON_Object* _nObj);
	//Internal use only
	void PopulateArray(JSON_Value* _array, float* value, unsigned int size);


	void WriteFloat(const char* name, float value);
	float ReadFloat(const char* name);

	void WriteString(const char* name, const char* value);
	const char* ReadString(const char* name);

	void WriteBool(const char* name, bool value);
	bool ReadBool(const char* name);

	void WriteInt(const char* name, int value);
	int ReadInt(const char* name);

	void WriteVector3(const char* name, float* value);
	float3 ReadVector3(const char* name);

	void WriteQuat(const char* name, float* value);
	Quat ReadQuat(const char* name);

	JSON_Array* ReadArray(const char* name);
	JSON_Object* ReadObject(const char* name);
	JSON_Value* ReadValue(const char* name);

	JSON_Object* nObj;
};