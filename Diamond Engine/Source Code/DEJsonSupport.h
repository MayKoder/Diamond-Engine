#pragma once

#include "Globals.h"
#include"parson/parson.h"
#include"MathGeoLib/include/Math/float2.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/float4.h"
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

	void WriteVector2(JSON_Object* obj, const char* name, float* value);
	float2 ReadVector2(JSON_Object* obj, const char* name);

	void WriteVector3(JSON_Object* obj, const char* name, float* value);
	float3 ReadVector3(JSON_Object* obj, const char* name);

	void WriteVector4(JSON_Object* obj, const char* name, float* value);
	float4 ReadVector4(JSON_Object* obj, const char* name);

	void WriteQuat(JSON_Object* obj, const char* name, float* value);
	Quat ReadQuat(JSON_Object* obj, const char* name);
}

struct DEConfig
{
	DEConfig();
	//DEConfig(const char* buffer);
	DEConfig(JSON_Object* _nObj);

	uint Save(char** buffer);

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

	void WriteVector2(const char* name, float* value);
	float2 ReadVector2(const char* name);

	void WriteVector3(const char* name, float* value);
	float3 ReadVector3(const char* name);

	void WriteVector4(const char* name, float* value);
	float4 ReadVector4(const char* name);

	void WriteQuat(const char* name, float* value);
	Quat ReadQuat(const char* name);

	JSON_Array* ReadArray(const char* name);
	JSON_Object* ReadObject(const char* name);
	JSON_Value* ReadValue(const char* name);

	JSON_Object* nObj;
	JSON_Value* root;
};

struct DEConfigArray
{
	DEConfigArray();

	void AddObject(DEConfig& object);
	int Size();

	JSON_Array* _array;
	JSON_Value* _value;
};