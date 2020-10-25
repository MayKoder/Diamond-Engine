#pragma once

class GameObject;

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

	Type type;

protected:
	//const char* name;
	bool active;
	GameObject* gameObject;

};