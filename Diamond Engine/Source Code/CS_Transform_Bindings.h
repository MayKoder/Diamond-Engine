#pragma once

#include "Globals.h"
#include "Application.h"

#include"GameObject.h"
#include"DETime.h"
#include"RE_Mesh.h"

#include"CO_MeshRenderer.h"
#include"CO_Script.h"
#include"CO_Transform.h"

#include"MO_Scene.h"
#include"MO_Input.h"
#include"MO_ResourceManager.h"

#include"GameObject.h"
#include"MathGeoLib/include/Math/float3.h"

#pragma region Internals
//-------------------------------------------- Internals -----------------------------------------------//
void CSLog(MonoObject* x)
{
	if (x == NULL)
		return;

	MonoString* string = mono_object_to_string(x, NULL);
	LOG(LogType::L_WARNING, mono_string_to_utf8(string));
}

int GetKey(MonoObject* x)
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetKey(*(int*)mono_object_unbox(x));

	return false;
}
int MouseX()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetMouseXMotion();
}
int MouseY()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetMouseYMotion();
}

void CSCreateGameObject(MonoObject* name, MonoObject* position)
{
	if (EngineExternal == nullptr)
		return;

	char* p = mono_string_to_utf8(mono_object_to_string(name, NULL));
	GameObject* go = EngineExternal->moduleScene->CreateGameObject(p, EngineExternal->moduleScene->root);
	mono_free(p);

	float3 posVector = M_MonoManager::UnboxVector(position);

	go->transform->position = posVector;
	go->transform->updateTransform = true;
}

void UpdateCppRotation(int _UID, MonoObject* rotation)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID);
	if (workGO == nullptr)
		return;

	Quat newRot = EngineExternal->moduleMono->UnboxQuat(rotation);

	workGO->transform->SetTransformMatrix(workGO->transform->position, newRot, workGO->transform->localScale);
	workGO->transform->updateTransform = true;

}
void UpdateCppScale(int _UID, MonoObject* scale)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID);
	if (workGO == nullptr)
		return;


	float3 newScale = EngineExternal->moduleMono->UnboxVector(scale);

	workGO->transform->SetTransformMatrix(workGO->transform->position, workGO->transform->rotation, newScale);
	workGO->transform->updateTransform = true;

}

MonoObject* SendPosition(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	if (EngineExternal == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(obj));

	uint _UID = 0;
	mono_field_get_value(obj,  mono_class_get_field_from_name(mono_object_get_class(obj) ,"UID"), &_UID);

	MonoClass* vecClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Vector3");
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return EngineExternal->moduleMono->Float3ToCS(EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID)->transform->position); //Use this method to send class types
}
void RecievePosition(MonoObject* obj, MonoObject* secObj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	if (EngineExternal == nullptr)
		return;

	float3 omgItWorks = EngineExternal->moduleMono->UnboxVector(secObj);

	uint _UID = 0;
	mono_field_get_value(obj, mono_class_get_field_from_name(mono_object_get_class(obj), "UID"), &_UID);
	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (workGO->transform)
	{
		workGO->transform->SetTransformMatrix(omgItWorks, workGO->transform->rotation, workGO->transform->localScale);
		workGO->transform->updateTransform = true;
	}
}

float GetDT() //TODO: Can we do this without duplicating code? plsssss
{
	return DETime::deltaTime;
}

void Destroy(MonoObject* go)
{
	MonoClass* klass = mono_object_get_class(go);
	int uid;
	mono_field_get_value(go, mono_class_get_field_from_name(klass, "UID"), &uid);

	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, uid);
	if (workGO == nullptr)
		return;

	workGO->Destroy();

}

MonoObject* GetForward(MonoObject* go)
{
	if (EngineExternal == nullptr || C_Script::runningScript == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(go));

	uint _UID = 0;
	mono_field_get_value(go, mono_class_get_field_from_name(mono_object_get_class(go), "UID"), &_UID);
	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID);

	MonoClass* vecClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Vector3");
	return EngineExternal->moduleMono->Float3ToCS(workGO->transform->GetForward());
}

void CreateBullet(MonoObject* position, MonoObject* rotation, MonoObject* scale) //TODO: We really need prefabs
{
	if (EngineExternal == nullptr)
		return;

	GameObject* go = EngineExternal->moduleScene->CreateGameObject("Bullet", EngineExternal->moduleScene->root);

	float3 posVector = M_MonoManager::UnboxVector(position);
	Quat rotQuat = M_MonoManager::UnboxQuat(rotation);
	float3 scaleVector = M_MonoManager::UnboxVector(scale);

	go->transform->SetTransformMatrix(posVector, rotQuat, scaleVector);
	go->transform->updateTransform = true;


	C_MeshRenderer* meshRenderer =  dynamic_cast<C_MeshRenderer*>(go->AddComponent(Component::Type::MeshRenderer));

	ResourceMesh* test = dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(1736836885, "Library/Meshes/1736836885.mmh"));
	meshRenderer->SetRenderMesh(test);

	go->AddComponent(Component::Type::Script, "BH_Bullet");
}

#pragma endregion