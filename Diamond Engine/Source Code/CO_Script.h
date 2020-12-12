#pragma once
#include "Component.h"
#include<vector>
#include<string>

class GameObject;
typedef struct _MonoClassField MonoClassField;

class C_Script : public Component
{
public:

	C_Script(GameObject* _gm);
	virtual ~C_Script();

	void Update() override;

	bool OnEditor() override;

	std::vector<std::string> methods;
	std::vector<MonoClassField*> fields;
};