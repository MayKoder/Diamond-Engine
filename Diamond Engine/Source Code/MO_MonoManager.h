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

#define USER_SCRIPTS_NAMESPACE ""
#define DE_SCRIPTS_NAMESPACE "DiamondEngine"

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
	SerializedField(MonoClassField* _field, MonoObject* _object);

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
	update_status Update(float dt) override;
	bool CleanUp() override;

	static Quat UnboxQuat(MonoObject* _obj);
	static float3 UnboxVector(MonoObject* _obj);

	static void LoadFieldData(SerializedField& _field, MonoObject* _object);
	static void DebugAllFields(const char* className, std::vector<SerializedField>& _data, MonoObject* obj);
	static void DebugAllMethods(const char* nsName, const char* className, std::vector<std::string>& _data);


	MonoObject* QuatToCS(Quat& inVec) const;
	MonoObject* Float3ToCS(float3& inVec) const;
	MonoObject* GoToCSGO(GameObject* inGo) const;

public:
	MonoDomain* domain;
	MonoAssembly* assembly;
	MonoMethod* updateMethod;
	MonoImage* image;

	std::vector<MonoClass*> userScripts;

};