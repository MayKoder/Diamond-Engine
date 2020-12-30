#pragma once
#include "Component.h"
#include<vector>
#include<string>

#include"MO_MonoManager.h"
#include <mono/metadata/object-forward.h>

class GameObject;
typedef struct _MonoClassField MonoClassField;

class C_Script : public Component
{
public:

	C_Script(GameObject* _gm, const char* scriptName);
	virtual ~C_Script();

	void Update() override;

	bool OnEditor() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(JSON_Object* nObj) override;

	void SetField(MonoClassField* field, GameObject* value);
	void DropField(SerializedField& fieldName, const char* dropType);

	void LoadScriptData(const char*);

	std::vector<std::string> methods;
	std::vector<SerializedField> fields;

	MonoObject* coreObject;
	MonoMethod* updateMethod;

	uint32_t noGCobject;

	static C_Script* runningScript;
};