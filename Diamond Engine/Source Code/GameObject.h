#pragma once

#include<vector>
#include<string>
#include <map>

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
	Component* GetComponent(Component::TYPE _type, const char* scriptName = nullptr);
	std::vector<Component*> GetComponentsOfType(Component::TYPE type);

	void RecursiveUIDRegeneration();
	void RecursiveUIDRegenerationSavingReferences(std::map<uint, GameObject*>& gameObjects);

	bool isActive() const;

	void Enable();
	void Disable();

	void EnableTopDown();
	void DisableTopDown();

	bool IsRoot();

	void Destroy();

	void SaveToJson(JSON_Array*);
	void LoadFromJson(JSON_Object*);

	void LoadComponents(JSON_Array* componentArray);
	void RemoveComponent(Component* ptr);

	void ChangeParent(GameObject*);
	bool IsChild(GameObject*);

	void RemoveChild(GameObject*);
	void CollectChilds(std::vector<GameObject*>& vector);

	bool CompareTag(const char* _tag);

	template<typename A>
	A* GetComponent()
	{
		return (A*)GetComponent(A::GetType());
	}

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
	uint prefabID;

	char tag[32];
	char layer[32];

private:
	Component* dumpComponent;

};