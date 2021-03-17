#pragma once

class GameObject;

//ERROR: Remove this include from here
#include"DEJsonSupport.h"
#include<string>
#include <map>

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
		RIGIDBODY,
		COLLIDER,
		ANIMATOR,
		NAVIGATION,
		BOXCOLLIDER,
		MESHCOLLIDER,
		PARTICLE_SYSTEM,
		BILLBOARD,
		COUNT
	};

	Component(GameObject* _gm);
	virtual ~Component();

	virtual void Enable();
	virtual void Disable();

	virtual void Update();
	virtual void PostUpdate();

	virtual void OnRecursiveUIDChange(std::map < uint, GameObject*> gameObjects);

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

	inline std::string GetName() {
		return name;
	}

	TYPE type;

protected:
	//const char* name;
	bool active;
	GameObject* gameObject;
	std::string name;
};