#pragma once

#include<vector>
#include<string>

#include "Component.h"
#include"parson/parson.h"

class C_Transform;
struct SerializedField;

class GameObject
{
public:

	GameObject(const char*, GameObject* parent, int _uid = -1);
	virtual ~GameObject();

	void Update();
	void PostUpdate();

	Component* AddComponent(Component::TYPE _type, const char* params = nullptr);
	Component* GetComponent(Component::TYPE _type);

	void RecursiveUIDRegeneration();

	bool isActive() const;

	void Enable();
	void Disable();
	bool IsRoot();

	void Destroy();

	void SaveToJson(JSON_Array*);
	void LoadFromJson(JSON_Object*);

	void LoadComponents(JSON_Array* componentArray);
	void RemoveComponent(Component* ptr);

	void ChangeParent(GameObject*);
	bool IsChild(GameObject*);

	void RemoveChild(GameObject*);

	GameObject* parent;
	C_Transform* transform;

	std::vector<GameObject*> children;

	std::vector<Component*> components;

	std::vector<SerializedField*> csReferences;

public:
	std::string name;
	bool active, isStatic;

	//TODO: Temporal tree display bool
	bool showChildren;
	bool toDelete;

	int UID;

private:
	Component* dumpComponent;

};