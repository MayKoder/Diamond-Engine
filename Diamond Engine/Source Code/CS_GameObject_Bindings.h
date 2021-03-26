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