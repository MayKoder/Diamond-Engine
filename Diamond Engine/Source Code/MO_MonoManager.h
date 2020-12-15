#pragma once
#include "Module.h"

#include <mono/utils/mono-publib.h>
#include"MathGeoLib/include/Math/float3.h"

#include<mono/metadata/object-forward.h>
#include <mono/metadata/blob.h>

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoClassField MonoClassField;

class GameObject;

union FieldValue
{
	int iValue;
	float fValue;
	bool bValue;
	char strValue[50];
	GameObject* goValue;
};
struct SerializedField
{
	SerializedField();
	SerializedField(MonoClassField* _field, void* _value);

	MonoClassField* field;
	MonoTypeEnum type = MonoTypeEnum::MONO_TYPE_END;
	FieldValue fiValue;
};

class M_MonoManager : public Module
{
public:
	M_MonoManager(Application* app, bool start_enabled = true);
	virtual ~M_MonoManager();

	bool Init() override;
	bool CleanUp() override;

public:
	MonoDomain* domain;
	MonoAssembly* assembly;
	MonoObject* coreObject;
	MonoMethod* updateMethod;
	MonoImage* image;

	static float3 UnboxVector(MonoObject* _obj);
	static Quat UnboxQuat(MonoObject* _obj);

	static void DebugAllFields(const char* className, std::vector<SerializedField>& _data);
	static void DebugAllMethods(const char* className, std::vector<std::string>& _data);

	MonoObject* GoToCSGO(GameObject* inGo) const;
	MonoObject* Float3ToCS(float3& inVec) const;
	MonoObject* QuatToCS(Quat& inVec) const;
};