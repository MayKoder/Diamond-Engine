#pragma once

#include "Application.h"
#include "MO_MonoManager.h"
#include "GameObject.h"
#include "CS_Transform_Bindings.h"

#include "CO_RigidBody.h"

MonoString* GetTag(MonoObject* cs_Object)
{
	GameObject* cpp_gameObject = EngineExternal->moduleMono->GameObject_From_CSGO(cs_Object);

	return mono_string_new(EngineExternal->moduleMono->domain, cpp_gameObject->tag);
}

void SetVelocity(MonoObject* cs_GameObject, MonoObject* cs_Velocity)
{
	float3 velocity;
	GameObject* cpp_gameObject = EngineExternal->moduleMono->GameObject_From_CSGO(cs_GameObject);
	velocity = M_MonoManager::UnboxVector(cs_Velocity);
	
	C_RigidBody* rigidbody = dynamic_cast<C_RigidBody*>(cpp_gameObject->GetComponent(Component::TYPE::RIGIDBODY));
	
	if (rigidbody)
	rigidbody->SetLinearVelocity(velocity);

}


void AddForce(MonoObject* cs_GameObject, MonoObject* cs_Force)
{
	float3 force;
	GameObject* cpp_gameObject = EngineExternal->moduleMono->GameObject_From_CSGO(cs_GameObject);
	force = M_MonoManager::UnboxVector(cs_Force);

	C_RigidBody* rigidbody = dynamic_cast<C_RigidBody*>(cpp_gameObject->GetComponent(Component::TYPE::RIGIDBODY));
	
	if(rigidbody)
	rigidbody->AddForce(force);

}


MonoObject* FindObjectWithName(MonoString* name) {

	std::vector<GameObject*> gameObjectVec;
	EngineExternal->moduleScene->root->CollectChilds(gameObjectVec);

	if (name == NULL) {
		assert("The name you passed is null. >:/");
		return nullptr;
	}

	char* _name = mono_string_to_utf8(name);
	mono_free(_name);

	for (int i = 0; i < gameObjectVec.size(); i++) {

		if(strcmp(gameObjectVec[i]->name.c_str(), _name) == 0){
		
			return EngineExternal->moduleMono->GoToCSGO(gameObjectVec[i]);

		}

	}

	assert("The object you searched for doesn't exist. :/");
	return nullptr;

}


void CS_SetParent(MonoObject* gameObject, MonoObject* newParent) {

	GameObject* cpp_gameObject = EngineExternal->moduleMono->GameObject_From_CSGO(gameObject);
	GameObject* cpp_parent = EngineExternal->moduleMono->GameObject_From_CSGO(newParent);
	cpp_gameObject->ChangeParent(cpp_parent);

}
