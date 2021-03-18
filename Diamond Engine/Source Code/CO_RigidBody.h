#pragma once

#include "PhysX/include/PxPhysicsAPI.h"
#include"MathGeoLib/include/Math/float4x4.h"

#include "Component.h"


class GameObject;
class C_Collider;
class C_Transform;
class C_MeshRenderer;

class C_RigidBody : public Component
{
public:

	C_RigidBody();
	C_RigidBody(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/);
	virtual ~C_RigidBody();

	void PostUpdate() override;
	void Step();

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE
	inline void EnableGravity(bool enable) {rigid_dynamic->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !enable); };
	inline void EnableKinematic(bool enable) { rigid_dynamic->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, enable); };

	inline void SetMass(float mass) {rigid_dynamic->setMass(mass); };
	inline float GetMass() const { return rigid_dynamic->getMass(); };

	inline void SetDensity(float density) { physx::PxRigidBodyExt::updateMassAndInertia(*rigid_dynamic, density); };
	inline float GetDensity() const { return density; };

	inline void AddForce(float3 force) { rigid_dynamic->addForce({ force.x, force.y, force.z }); };
	inline float3 GetForce() const { return force; };

	inline void AddTorque(float3 torque) { rigid_dynamic->addTorque({ torque.x, torque.y, torque.z }); };
	inline float3 GetTorque() const { return torque; };

	//------------Velocity------------//
	inline void SetLinearVelocity(float3 linear_velocity) { rigid_dynamic->setLinearVelocity({ linear_velocity.x, linear_velocity.y, linear_velocity.z }); };
	inline physx::PxVec3 GetLinearVelocity() { return rigid_dynamic->getLinearVelocity(); };

	inline void SetAngularVelocity(float3 angular_velocity) { rigid_dynamic->setAngularVelocity({ angular_velocity.x, angular_velocity.y, angular_velocity.z }); };
	inline physx::PxVec3 GetAngularVeloctity() { return rigid_dynamic->getAngularVelocity(); };

	//------------Damping------------//
	inline void SetAngularDamping(float angular_Damping) { rigid_dynamic->setAngularDamping(angular_Damping); };
	inline float GetAngularDamping() { return rigid_dynamic->getAngularDamping(); };

	inline void SetLinearDamping(float linear_Damping) { rigid_dynamic->setLinearDamping(linear_Damping); };
	inline float GetLinearDamping() { return rigid_dynamic->getLinearDamping(); };

	//------------Axis Locking------------//
	inline void LockLinearX(bool enable) { rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, enable); };
	inline void LockLinearY(bool enable) { rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, enable); };
	inline void LockLinearZ(bool enable) { rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, enable); };

	inline void LockAngularX(bool enable) { rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, enable); };
	inline void LockAngularY(bool enable) { rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, enable); };
	inline void LockAngularZ(bool enable) { rigid_dynamic->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, enable); };


public:

	C_Transform* goTransform;
	std::vector<Component*> collider_info;

	C_MeshRenderer* mesh;
	
	physx::PxRigidDynamic* rigid_dynamic;

	bool use_gravity = true;
	bool use_kinematic = false;

	bool lock_linearX = false, lock_linearY = false, lock_linearZ = false;
	bool lock_angularX = false, lock_angularY = false, lock_angularZ = false;

	float mass = 10.0f;
	float density = 1.0f;

	float3 force = float3::zero;
	float3 torque = float3::zero;

	float3 linear_vel = float3::zero;
	float3 angular_vel = float3::zero;

	float linear_damping = 0.0f;
	float angular_damping = 0.0f;
	
	float4x4 global_to_pivot;
	float3 offset;
};