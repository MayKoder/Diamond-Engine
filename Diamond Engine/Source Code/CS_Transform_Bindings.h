#pragma once

#include "Globals.h"
#include "Application.h"

#include "GameObject.h"
#include "DETime.h"
#include "RE_Mesh.h"

#include "CO_MeshRenderer.h"
#include "CO_Script.h"
#include "CO_Transform.h"
#include "CO_Collider.h"
#include "CO_RigidBody.h"

#include "MO_Input.h"
#include "MO_Scene.h"
#include "MO_ResourceManager.h"

#include "GameObject.h"
#include "MathGeoLib/include/Math/float3.h"

Component* DECS_CompToComp(MonoObject* obj)
{
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Transform");

	mono_field_get_value(obj, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<Component*>(ptr);
}

Component* DECS_CompToGameObject(MonoObject* obj)
{
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "DiamondComponent");

	mono_field_get_value(obj, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<Component*>(ptr);
}

MonoObject* CS_Component_Get_GO(MonoObject* thisRef)
{
	//return EngineExternal->modul
	return nullptr;
}

//template<typename A>
MonoObject* CS_GetComponent(MonoObject* ref, MonoString* type, int inputType)
{
	char* name = mono_string_to_utf8(type);
	std::string complete(name);

	std::string np(complete);
	np = np.substr(np.find_first_of('.') + 1);

	std::string klass(complete);
	klass = klass.substr(0, klass.find_first_of('.'));

	MonoClass* cmpClass = mono_class_from_name(EngineExternal->moduleMono->image, klass.c_str(), np.c_str());
	MonoObject* ret = mono_object_new(EngineExternal->moduleMono->domain, cmpClass);

	//Get type from unity
	Component::TYPE sType = static_cast<Component::TYPE>(inputType);
	Component* transform = EngineExternal->moduleMono->GameObject_From_CSGO(ref)->GetComponent(sType);

	//Get type
	MonoClassField* field = mono_class_get_field_from_name(cmpClass, "pointer");

	uintptr_t goPtr = reinterpret_cast<uintptr_t>(transform);
	mono_field_set_value(ret, field, &goPtr);


	//Component* willThisWork = DECS_CompToComp(ret);
	//MonoObject* ret = mono_object_new(EngineExternal->moduleMono->domain, mono_class_from_mono_type(mono_type_create_from_typespec(EngineExternal->moduleMono->image, type)));

	mono_free(name);

	return ret;
}

//------//
MonoObject* DE_Box_Vector(MonoObject* obj, const char* type, bool global)
{
	if (EngineExternal == nullptr)
		return nullptr;

	//const char* name = mono_class_get_name(mono_object_get_class(obj));

	float3 value;
	C_Transform* transform = dynamic_cast<C_Transform*>(DECS_CompToComp(obj));

	if (strcmp(type, "POSITION") == 0)
	{
		(global == true) ? value = transform->globalTransform.TranslatePart() : value = transform->position;
	}
	else
	{
		(global == true) ? value = transform->globalTransform.GetScale() : value = transform->localScale;
	}

	return EngineExternal->moduleMono->Float3ToCS(value);
}

MonoObject* DE_Box_Quat(MonoObject* obj, bool global)
{
	if (EngineExternal == nullptr)
		return nullptr;

	const char* name = mono_class_get_name(mono_object_get_class(obj));

	Quat value;
	C_Transform* workTrans = dynamic_cast<C_Transform*>(DECS_CompToComp(obj));

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
	C_Transform* workTrans = dynamic_cast<C_Transform*>(DECS_CompToComp(obj));

	if (workTrans)
	{
		workTrans->position = omgItWorks;
		//workGO->transform->SetTransformMatrix(omgItWorks, workGO->transform->rotation, workGO->transform->localScale);
		workTrans->updateTransform = true;
	}
}
MonoObject* GetForward(MonoObject* go)
{
	if (EngineExternal == nullptr || C_Script::runningScript == nullptr)
		return nullptr;

	C_Transform* trans = dynamic_cast<C_Transform*>(DECS_CompToComp(go));

	MonoClass* vecClass = mono_class_from_name(EngineExternal->moduleMono->image, DE_SCRIPTS_NAMESPACE, "Vector3");
	return EngineExternal->moduleMono->Float3ToCS(trans->GetForward());
}
MonoObject* GetRight(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return nullptr;

	C_Transform* trans = dynamic_cast<C_Transform*>(DECS_CompToComp(go));

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
	C_Transform* transform = dynamic_cast<C_Transform*>(DECS_CompToComp(obj)); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

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
	C_Transform* transform = dynamic_cast<C_Transform*>(DECS_CompToComp(obj)); //TODO IMPORTANT: First parameter is the object reference, use that to find UID

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


	C_MeshRenderer* meshRenderer =  dynamic_cast<C_MeshRenderer*>(go->AddComponent(Component::TYPE::MESH_RENDERER));

	ResourceMesh* test = 
		dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(965117995, Resource::Type::MESH));
	meshRenderer->SetRenderMesh(test);

	go->AddComponent(Component::TYPE::SCRIPT, "BH_Bullet");

	C_Collider* col = dynamic_cast<C_Collider*>(go->AddComponent(Component::TYPE::Collider));
	col->SetTrigger(true);

	C_RigidBody* rb = dynamic_cast<C_RigidBody*>(go->GetComponent(Component::TYPE::RigidBody));
	rb->EnableGravity(false);
	rb->EnableKinematic(false);

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