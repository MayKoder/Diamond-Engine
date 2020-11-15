#pragma once

class GameObject;

//ERROR: Remove this include from here
#include"parson/parson.h"

class Component
{
public:

	enum class Type
	{
		None,
		Transform,
		MeshRenderer,
		Material
	};

	Component(GameObject* _gm);
	virtual ~Component();

	virtual void Enable();
	virtual void Disable();

	virtual void Update();

	virtual void OnEditor();

	//Maybe return JSON_Value*?
	virtual void SaveComponent(JSON_Object* nObj);

	inline bool IsActive() {
		return active;
	}

	Type type;

protected:
	//const char* name;
	bool active;
	GameObject* gameObject;

};