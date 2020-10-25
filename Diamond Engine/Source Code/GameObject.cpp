#include "GameObject.h"
#include "Component.h"

#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Material.h"

GameObject::GameObject(const char* _name) : parent(nullptr), name(_name), showChildren(false),
active(true), isStatic(false)
{
	AddComponent(Component::Type::Transform);
}

GameObject::~GameObject()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		delete components[i];
		components[i] = nullptr;
	}
	components.clear();

	for (size_t i = 0; i < children.size(); i++)
	{
		delete children[i];
		children[i] = nullptr;
	}
	children.clear();
}

void GameObject::Update()
{
	for (size_t i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

Component* GameObject::AddComponent(Component::Type _type)
{

	Component* ret = nullptr;

	switch (_type)
	{
	case Component::Type::Transform:
		/*if(transform == nullptr)*/
			ret = new C_Transform(this);
		break;
	case Component::Type::MeshRenderer:
		ret = new C_MeshRenderer(this);
		break;
	case Component::Type::Material:
		ret = new C_Material(this);
		break;
	}

	components.push_back(ret);

	return ret;
}

Component* GameObject::GetComponent(Component::Type _type)
{
	for (size_t i = 0; i < components.size(); i++)
	{
		if (components[i]->type == _type)
			return components[i];
	}

	return nullptr;
}
