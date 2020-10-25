#pragma once

#include<vector>
#include<string>

#include "Component.h"
//#include "C_Transform.h"

class GameObject
{
public:
	GameObject(const char*);
	virtual ~GameObject();

	void Update();
	Component* AddComponent(Component::Type _type);
	Component* GetComponent(Component::Type _type);

	GameObject* parent;
	//C_Transform* transform;

	std::vector<GameObject*> children;

	std::vector<Component*> components;

public:
	std::string name;
	bool active, isStatic;

	//TODO: Temporal tree display bool
	bool showChildren;

};