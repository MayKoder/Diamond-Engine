#pragma once

class GameObject;

//ERROR: Remove this include from here
#include"DEJsonSupport.h"
#include<string>

class Component
{
public:

	enum class TYPE
	{
		NONE,
		TRANSFORM,
		MESH_RENDERER,
		MATERIAL,
		CAMERA,
		SCRIPT,
		TRANSFORM_2D,
		BUTTON,
		CHECKBOX,
		TEXT_UI,
		CANVAS,
		IMAGE_2D,
		AUDIO_LISTENER,
		AUDIO_SOURCE,
    RigidBody,
		Collider,
		COUNT
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

	TYPE type;

protected:
	//const char* name;
	bool active;
	GameObject* gameObject;
	std::string name;

};