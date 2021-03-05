#pragma once

#include "Globals.h"
#include "Application.h"

#include "GameObject.h"
#include "DETime.h"
#include "RE_Mesh.h"

#include "CO_MeshRenderer.h"
#include "CO_Script.h"
#include "CO_Transform.h"

#include "MO_Input.h"
#include "MO_Scene.h"
#include "MO_ResourceManager.h"

#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

//------//
MonoObject* DE_Box_Vector(MonoObject* obj, const char* type, bool global)
{
	if (EngineExternal == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(obj));

	float3 value;
	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(obj);

	if (strcmp(type, "POSITION") == 0)
	{
		(global == true) ? value = workGO->transform->globalTransform.TranslatePart() : value = workGO->transform->position;
	}
	else
	{
		(global == true) ? value = workGO->transform->globalTransform.GetScale() : value = workGO->transform->localScale;
	}

	return EngineExternal->moduleMono->Float3ToCS(value);
}
MonoObject* DE_Box_Quat(MonoObject* obj, bool global)
{
	if (EngineExternal == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(obj));

	Quat value;
	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(obj);

	(global == true) ? value = workGO->transform->globalTransform.RotatePart().ToQuat().Normalized() : value = workGO->transform->rotation;


	return EngineExternal->moduleMono->QuatToCS(value);
}

#pragma region Internals
//-------------------------------------------- Internals -----------------------------------------------//
void CSLog(MonoString* x)
{
	if (x == NULL)
		return;

	char* msg = mono_string_to_utf8(x);
	LOG(LogType::L_WARNING, msg);
	mono_free(msg);
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

MonoObject* SendPosition(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return DE_Box_Vector(obj, "POSITION", false); //Use this method to send class types
}
void RecievePosition(MonoObject* obj, MonoObject* secObj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	if (EngineExternal == nullptr)
		return;

	float3 omgItWorks = EngineExternal->moduleMono->UnboxVector(secObj);
	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (workGO->transform)
	{
		workGO->transform->position = omgItWorks;
		//workGO->transform->SetTransformMatrix(omgItWorks, workGO->transform->rotation, workGO->transform->localScale);
		workGO->transform->updateTransform = true;
	}
}
MonoObject* GetForward(MonoObject* go)
{
	if (EngineExternal == nullptr || C_Script::runningScript == nullptr)
		return nullptr;

	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	MonoClass* vecClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Vector3");

	return EngineExternal->moduleMono->Float3ToCS(workGO->transform->GetForward());
}
MonoObject* GetRight(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return nullptr;

	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	MonoClass* vecClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Vector3");
	return EngineExternal->moduleMono->Float3ToCS(workGO->transform->GetRight());
}

MonoObject* SendRotation(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	return DE_Box_Quat(obj, false); //Use this method to send class types
}
void RecieveRotation(MonoObject* obj, MonoObject* secObj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	if (EngineExternal == nullptr)
		return;

	Quat omgItWorks = EngineExternal->moduleMono->UnboxQuat(secObj);
	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (workGO->transform)
	{
		//workGO->transform->SetTransformMatrix(workGO->transform->position, omgItWorks, workGO->transform->localScale);

		workGO->transform->rotation = omgItWorks.Normalized();
		workGO->transform->eulerRotation = omgItWorks.ToEulerXYZ() * RADTODEG;

		workGO->transform->updateTransform = true;
	}
}

MonoObject* SendScale(MonoObject* obj)
{
	return DE_Box_Vector(obj, "SCALE", false);
}
void RecieveScale(MonoObject* obj, MonoObject* secObj)
{
	if (EngineExternal == nullptr)
		return;

	float3 omgItWorks = EngineExternal->moduleMono->UnboxVector(secObj);
	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (workGO->transform)
	{
		//workGO->transform->SetTransformMatrix(workGO->transform->position, workGO->transform->rotation, omgItWorks);
		workGO->transform->localScale = omgItWorks;
		workGO->transform->updateTransform = true;
	}
}

float GetDT() //TODO: Can we do this without duplicating code? plsssss
{
	return DETime::deltaTime;
}

void Destroy(MonoObject* go)
{
	if (go == NULL)
		return;

	MonoClass* klass = mono_object_get_class(go);
	//const char* name = mono_class_get_name(klass);

	GameObject* workGO = EngineExternal->moduleMono->GameObject_From_CSGO(go);
	//GameObject* workGO = C_Script::runningScript->GetGO();
	if (workGO == nullptr) {
		LOG(LogType::L_ERROR, "AY LMAO CANT DELETE AYAYAYAYTA");
		return;
	}

	workGO->Destroy();
}


void CreateBullet(MonoObject* position, MonoObject* rotation, MonoObject* scale) //TODO: We really need prefabs
{
	if (EngineExternal == nullptr)
		return /*nullptr*/;

	GameObject* go = EngineExternal->moduleScene->CreateGameObject("Empty", EngineExternal->moduleScene->root);
	////go->name = std::to_string(go->UID);

	float3 posVector = M_MonoManager::UnboxVector(position);
	Quat rotQuat = M_MonoManager::UnboxQuat(rotation);
	float3 scaleVector = M_MonoManager::UnboxVector(scale);

	go->transform->SetTransformMatrix(posVector, rotQuat, scaleVector);
	go->transform->updateTransform = true;


	C_MeshRenderer* meshRenderer =  dynamic_cast<C_MeshRenderer*>(go->AddComponent(Component::TYPE::MESH_RENDERER));

	ResourceMesh* test = 
		dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(965117995, Resource::Type::MESH));
	meshRenderer->SetRenderMesh(test);

	go->AddComponent(Component::TYPE::SCRIPT, "BH_Bullet");

	/*return mono_gchandle_get_target(cmp->noGCobject);*/
}

//---------- GLOBAL GETTERS ----------//
MonoObject* SendGlobalPosition(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return DE_Box_Vector(obj, "POSITION", true); //Use this method to send class types
}
MonoObject* SendGlobalRotation(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return DE_Box_Quat(obj, true); //Use this method to send class types
}
MonoObject* SendGlobalScale(MonoObject* obj) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return DE_Box_Vector(obj, "SCALE", true); //Use this method to send class types
}

#pragma endregion