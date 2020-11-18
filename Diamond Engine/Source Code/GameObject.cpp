#include "GameObject.h"
#include "Component.h"

#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Material.h"
#include "C_Camera.h"

#include"MaykMath.h"
#include"parson/parson.h"
#include"DEJsonSupport.h"
#include <algorithm>

GameObject::GameObject(const char* _name, GameObject* parent, int _uid) : parent(parent), name(_name), showChildren(false),
active(true), isStatic(false), toDelete(false), UID(_uid), transform(nullptr), dumpComponent(nullptr)
{

	if(parent != nullptr)
		parent->children.push_back(this);

	transform = dynamic_cast<C_Transform*>(AddComponent(Component::Type::Transform));

	//TODO: Should make sure there are not duplicated ID's
	if(UID == -1)
		UID = MaykMath::Random(0, INT_MAX);
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
	if (dumpComponent != nullptr) 
	{
		components.erase(std::find(components.begin(), components.end(), dumpComponent));
		delete dumpComponent;
		dumpComponent = nullptr;
	}

	//transform->eulerRotation.y += 1.f;
	//transform->updateTransform = true;

	for (size_t i = 0; i < components.size(); i++)
	{
		if(components[i]->IsActive())
			components[i]->Update();
	}
}

Component* GameObject::AddComponent(Component::Type _type)
{

	assert(_type != Component::Type::None, "Can't create a NONE component");
	Component* ret = nullptr;

	//TODO: Make a way to add only 1 instance components like transform and camera
	switch (_type)
	{
	case Component::Type::Transform:
		if(transform == nullptr)
			ret = new C_Transform(this);
		break;
	case Component::Type::MeshRenderer:
		ret = new C_MeshRenderer(this);
		break;
	case Component::Type::Material:
		ret = new C_Material(this);
		break;
	case Component::Type::Camera:
		ret = new C_Camera(this);
		break;
	}

	if (ret != nullptr)
	{		
		ret->type = _type;
		components.push_back(ret);
	}

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

bool GameObject::isActive() const
{
	return active;
}

//void GameObject::ChangeActiveState()
//{
//	(active == true) ? Disable() : Enable();
//}

void GameObject::Enable()
{
	active = true;

	if (parent != nullptr)
		parent->Enable();
}

void GameObject::Disable()
{
	active = false;
	//for (size_t i = 0; i < children.size(); i++)
	//{
	//	children[i]->Disable();
	//}
}

bool GameObject::IsRoot()
{
	return (parent == nullptr) ? true : false;
}

void GameObject::Destroy()
{
	toDelete = true;
}

void GameObject::SaveToJson(JSON_Array* _goArray)
{
	JSON_Value* goValue = json_value_init_object();
	JSON_Object* goData = json_value_get_object(goValue);

	//Save all gameObject data
	json_object_set_string(goData, "name", name.c_str());

	DEJson::WriteVector3(goData, "Position", &transform->position[0]);
	DEJson::WriteQuat(goData, "Rotation", &transform->rotation.x);
	DEJson::WriteVector3(goData, "Scale", &transform->localScale[0]);

	DEJson::WriteInt(goData, "UID", UID);

	if (parent)
		DEJson::WriteInt(goData, "ParentUID", parent->UID);

	json_array_append_value(_goArray, goValue);

	//TODO: Move inside component base
	{
		//Save components
		JSON_Value* goArray = json_value_init_array();
		JSON_Array* jsArray = json_value_get_array(goArray);
		for (size_t i = 0; i < components.size(); i++)
		{
			JSON_Value* nVal = json_value_init_object();
			JSON_Object* nObj = json_value_get_object(nVal);

			components[i]->SaveData(nObj);
			json_array_append_value(jsArray, nVal);
		}
		json_object_set_value(goData, "Components", goArray);
	}

	for (size_t i = 0; i < children.size(); i++)
	{
		children[i]->SaveToJson(_goArray);
	}
}

void GameObject::LoadComponents(JSON_Array* componentArray)
{

	JSON_Object* jsComponent = nullptr;
	for (size_t i = 1; i < json_array_get_count(componentArray); i++)
	{
		jsComponent = json_array_get_object(componentArray, i);

		Component* comp = AddComponent((Component::Type)json_object_get_number(jsComponent, "Type"));
		comp->LoadData(jsComponent);

	}

}

void GameObject::RemoveComponent(Component* ptr)
{
	dumpComponent = ptr;
}

//TODO: WTF IS GOING ON WITH THE ARNAU BUG FFS
//Deparenting objects with deformations grows transforms
void GameObject::ChangeParent(GameObject* newParent)
{
	//GameObject* ret = nullptr;
	//ret = IsChild(newParent, ret);
	if (IsChild(newParent))
		return;

	parent->RemoveChild(this);	
	parent = newParent;
	parent->children.push_back(this);

	//TODO: This could be improved, you are setting up the local matrix 2 times
	transform->localTransform = parent->transform->globalTransform.Inverted() * transform->globalTransform;

	Quat _rot;
	float3 scale, pos;
	transform->localTransform.RotatePart().Decompose(_rot, scale);

	transform->SetTransformMatrix(transform->localTransform.TranslatePart(), _rot, scale);
	transform->updateTransform = true;

}

bool GameObject::IsChild(GameObject* _toFind)
{
	//_toFind is the node we are dropping to
	//If the parent is
	//this is the node we are reparenting
	//_tofind ==  this == if the parents of the node we are dropping is equal to "this", bad, dont reparent
	if (_toFind == nullptr)
		return false;

	if (_toFind == this)
	{
		return true;
	}
	else
	{
		IsChild(_toFind->parent);
	}
}

void GameObject::RemoveChild(GameObject* child)
{
	children.erase(std::find(children.begin(), children.end(), child));
}
