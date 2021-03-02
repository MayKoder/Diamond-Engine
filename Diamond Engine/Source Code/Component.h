#pragma once

class GameObject;

//ERROR: Remove this include from here
#include"DEJsonSupport.h"
#include<string>

class Component
{
public:

	enum class Type
	{
		None,
		Transform,
		MeshRenderer,
		Material,
		Camera,
		Script,
		AudioListener,
		AudioSource,
		Count,
		RigidBody,
		Collider,
	};

	Component(GameObject* _gm);
	virtual ~Component();

	virtual void Enable();
	virtual void Disable();

	virtual void Update();
	virtual void PostUpdate();

#ifndef STANDALONE
	virtual bool OnEditor();
#endif // !STANDALONE

	//Maybe return JSON_Value*?
	virtual void SaveData(JSON_Object* nObj);
	virtual void LoadData(DEConfig& nObj);

	inline bool IsActive() {
		return active;
	}

	inline GameObject* GetGO() {
		return gameObject;
	}

	Type type;

protected:
	//const char* name;
	bool active;
	GameObject* gameObject;
	std::string name;

};