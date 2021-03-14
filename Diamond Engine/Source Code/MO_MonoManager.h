#pragma once
#include "Module.h"

#include <mono/utils/mono-publib.h>
#include"MathGeoLib/include/Math/float3.h"

#include<mono/metadata/object-forward.h>
#include <mono/metadata/blob.h>
#include<mono/metadata/threads.h>

typedef struct _MonoDomain MonoDomain;
typedef struct _MonoAssembly MonoAssembly;
typedef struct _MonoClassField MonoClassField;

class GameObject;
class C_Script;

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
	SerializedField(MonoClassField* _field, MonoObject* _object, C_Script* parent);

	MonoClassField* field;
	MonoTypeEnum type = MonoTypeEnum::MONO_TYPE_END;
	FieldValue fiValue;
	C_Script* parentSC;
	std::string displayName;
};

class M_MonoManager : public Module
{
public:
	M_MonoManager(Application* app, bool start_enabled = true);
	virtual ~M_MonoManager();

	bool Init() override;

#ifndef STANDALONE
	void OnGUI() override;
	void ReCompileCS();
#endif // !STANDALONE

	bool CleanUp() override;

	static Quat UnboxQuat(MonoObject* _obj);
	static float3 UnboxVector(MonoObject* _obj);

	static void LoadFieldData(SerializedField& _field, MonoObject* _object);
	static void DebugAllMethods(const char* nsName, const char* className, std::vector<std::string>& _data);
	static void DebugAllFields(const char* className, std::vector<SerializedField>& _data, MonoObject* obj, C_Script* script, const char* namespace_name);

	void CreateAssetsScript(const char* localPath);
	void AddScriptToSLN(const char* scriptLocalPath);
	void RemoveScriptFromSLN(const char* scriptLocalPath);

	GameObject* GameObject_From_CSGO(MonoObject* goObj);
	GameObject* GameObject_From_CSCOMP(MonoObject* goComponent);

	MonoObject* QuatToCS(Quat& inVec) const;
	MonoObject* Float3ToCS(float3& inVec) const;
	MonoObject* GoToCSGO(GameObject* inGo) const;


public:
	MonoDomain* domain;
	MonoDomain* jitDomain;
	MonoAssembly* assembly;
	MonoImage* image;
	MonoThread* domainThread;

	std::vector<MonoClass*> userScripts;

private:
	void InitMono();

};