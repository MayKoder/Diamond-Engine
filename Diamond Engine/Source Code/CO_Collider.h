#pragma once
#include "Component.h"
#include "MO_Physics.h"

#include "physx/include/PxPhysicsAPI.h"

class GameObject;

class C_Collider : public Component
{
public:

	C_Collider();
	C_Collider(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/);
	virtual ~C_Collider();

	void Update() override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE
	physx::PxShape* colliderShape;
	physx::PxRigidStatic* rigidStatic;
	physx::PxMaterial* colliderMaterial;
	//ComponentTransform* transform;
	//ComponentRigidDynamic* rigidbody;

	float3 colliderPos = float3::zero;
	float3 colliderSize = float3::one; //BOX AND CAPSULE
	float3 colliderEuler = float3::zero;
	float  colliderRadius = 3.0f; //SPHERE AND CAPSULE 

	//GeometryType type;


private: 

};