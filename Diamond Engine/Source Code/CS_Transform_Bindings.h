#pragma once

#include "Globals.h"
#include "Application.h"

#include "GameObject.h"
#include "DETime.h"
#include "RE_Mesh.h"

#include "INC_AllComponents.h"

#include "MO_Input.h"
#include "MO_Scene.h"
#include "MO_ResourceManager.h"

#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

template<typename T>
T DECS_CompToComp(MonoObject* obj)
{
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_object_get_class(obj);
	const char* name = mono_class_get_name(goClass);

	mono_field_get_value(obj, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<T>(ptr);
}
GameObject* DECS_Comp_To_GameObject(MonoObject* component)
{
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_object_get_class(component);

	mono_field_get_value(component, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<Component*>(ptr)->GetGO();
}

MonoObject* CS_Component_Get_GO(MonoObject* thisRef) 
{
	return EngineExternal->moduleMono->GoToCSGO(DECS_Comp_To_GameObject(thisRef));
}

//template<typename A>
MonoObject* CS_GetComponent(MonoObject* ref, MonoString* type, int inputType) 
{

	Component::Type sType = static_cast<Component::Type>(inputType);

	char* name = mono_string_to_utf8(type);
	Component* component = EngineExternal->moduleMono->GameObject_From_CSGO(ref)->GetComponent(sType, name);
	mono_free(name);

	if (sType == Component::Type::Script) 
		return mono_gchandle_get_target(dynamic_cast<C_Script*>(component)->noGCobject);

	MonoClass* cmpClass = mono_object_get_class(ref);
	MonoObject* ret = mono_object_new(EngineExternal->moduleMono->domain, cmpClass);

	//Get type from unity

	//Get type
	MonoClassField* field = mono_class_get_field_from_name(cmpClass, "pointer");

	uintptr_t goPtr = reinterpret_cast<uintptr_t>(component);
	mono_field_set_value(ret, field, &goPtr);

	return ret;
}


//------//
MonoObject* DE_Box_Vector(MonoObject* obj, const char* type, bool global)
{
	if (EngineExternal == nullptr)
		return nullptr;

	//const char* name = mono_class_get_name(mono_object_get_class(obj));

	float3 value;
	C_Transform* workTrans = DECS_CompToComp<C_Transform*>(obj);

	if (strcmp(type, "POSITION") == 0)
	{
		(global == true) ? value = workTrans->globalTransform.TranslatePart() : value = workTrans->position;
	}
	else
	{
		(global == true) ? value = workTrans->globalTransform.GetScale() : value = workTrans->localScale;
	}

	return EngineExternal->moduleMono->Float3ToCS(value);
}
MonoObject* DE_Box_Quat(MonoObject* obj, bool global)
{
	if (EngineExternal == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(obj));

	Quat value;
	C_Transform* workTrans = DECS_CompToComp<C_Transform*>(obj);

	if (global == true)
	{
		float3 pos, scale;
		Quat globalRot;
		workTrans->globalTransform.Decompose(pos, globalRot, scale);

		value = globalRot;
	}
	else
	{
		value = workTrans->rotation;
	}


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
	C_Transform* workTrans = DECS_CompToComp<C_Transform*>(obj);

	if (workTrans)
	{
		workTrans->position = omgItWorks;
		//workGO->transform->SetTransformMatrix(omgItWorks, workGO->transform->rotation, workGO->transform->localScale);
		workTrans->updateTransform = true;
	}
}
MonoObject* GetForward(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return nullptr;

	C_Transform* trans = DECS_CompToComp<C_Transform*>(go);

	MonoClass* vecClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Vector3");
	return EngineExternal->moduleMono->Float3ToCS(trans->GetForward());
}
MonoObject* GetRight(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return nullptr;

	C_Transform* trans = DECS_CompToComp<C_Transform*>(go);

	MonoClass* vecClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Vector3");
	return EngineExternal->moduleMono->Float3ToCS(trans->GetRight());
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
	C_Transform* transform = DECS_CompToComp<C_Transform*>(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (transform)
	{
		//workGO->transform->SetTransformMatrix(workGO->transform->position, omgItWorks, workGO->transform->localScale);

		transform->rotation = omgItWorks.Normalized();
		transform->eulerRotation = omgItWorks.ToEulerXYZ() * RADTODEG;

		transform->updateTransform = true;
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
	C_Transform* transform = DECS_CompToComp<C_Transform*>(obj); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

	if (transform)
	{
		//workGO->transform->SetTransformMatrix(workGO->transform->position, workGO->transform->rotation, omgItWorks);
		transform->localScale = omgItWorks;
		transform->updateTransform = true;
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


	C_MeshRenderer* meshRenderer = dynamic_cast<C_MeshRenderer*>(go->AddComponent(Component::Type::MeshRenderer));

	ResourceMesh* test =
		dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(965117995, Resource::Type::MESH));
	meshRenderer->SetRenderMesh(test);

	go->AddComponent(Component::Type::Script, "BH_Bullet");

	/*return mono_gchandle_get_target(cmp->noGCobject);*/
}

//---------- GLOBAL GETTERS ----------//
MonoObject* SendGlobalPosition(MonoObject* transform) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return DE_Box_Vector(transform, "POSITION", true); //Use this method to send class types
}
MonoObject* SendGlobalRotation(MonoObject* transform) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return DE_Box_Quat(transform, true); //Use this method to send class types
}
MonoObject* SendGlobalScale(MonoObject* transform) //Allows to send float3 as "objects" in C#, should find a way to move Vector3 as class
{
	//return mono_value_box(EngineExternal->moduleMono->domain, vecClass, EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position)); //Use this method to send "object" types
	return DE_Box_Vector(transform, "SCALE", true); //Use this method to send class types
}

#pragma endregion