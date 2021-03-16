#pragma once
#include "Component.h"
#include "MO_Physics.h"

#include "physx/include/PxPhysicsAPI.h"

class GameObject;
class C_Transform;
class C_RigidBody;
class C_MeshRenderer;

class C_Collider : public Component
{
public:

	C_Collider();
	C_Collider(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/);
	virtual ~C_Collider();

	void Update() override;

	void SetPosition(float3 position);
	inline float3 GetPosition() { return colliderPos; };

	void SetRotation(Quat rotation);
	inline float3 GetEuler() { return colliderEuler; };

	void SetScale(float3 scale, float radius = 1.0f);
	inline float3 GetScale() { return colliderSize; };

	void SetTrigger(bool enable);

	physx::PxTransform SetLocalPose(physx::PxTransform transform);

	void UpdateValues();

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;
#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE
public:
	bool isTrigger;

	physx::PxShape* colliderShape;
	physx::PxRigidStatic* rigidStatic;
	physx::PxMaterial* colliderMaterial;
	C_Transform* transform;
	C_RigidBody* rigidbody;
	C_MeshRenderer* mesh;

	float4x4 localTransform;
	float3 colliderPos = float3::zero;
	float3 colliderSize = float3::one; //BOX AND CAPSULE
	float3 colliderEuler = float3::zero;
	float  colliderRadius = 3.0f; //SPHERE AND CAPSULE 



};