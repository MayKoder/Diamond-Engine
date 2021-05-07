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


	static inline Type GetType() { return Type::Script; }; //This will allow us to get the type from a template

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	void SetField(MonoClassField* field, GameObject* value);
	void DropField(SerializedField& fieldName, const char* dropType);

	void LoadScriptData(const char*);

private:
#ifndef STANDALONE
	bool OnEditor() override;
	void DisplayField(SerializedField& field, const char* dropType);
#endif // !STANDALONE

public:
	std::vector<std::string> methods;
	std::vector<SerializedField> fields;

	MonoMethod* updateMethod;
	uint32_t noGCobject;

	static C_Script* runningScript;
};