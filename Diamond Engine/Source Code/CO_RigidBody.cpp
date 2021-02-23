#include "CO_RigidBody.h"
#include "CO_Collider.h"
#include "CO_Transform.h"

#include "MO_Physics.h"
#include "Globals.h"
#include <vector>
#include"GameObject.h"
#include"Application.h"
#include"MO_Editor.h"


C_RigidBody::C_RigidBody() : Component(nullptr)
{


	name = "Rigidbody";
}


C_RigidBody::C_RigidBody(GameObject* _gm): Component(_gm)

{
	goTransform = dynamic_cast<C_Transform*>(_gm->GetComponent(Component::Type::Transform));
	//collider_info = _gm->GetCollider();

	collider_info = nullptr;
	EngineExternal->modulePhysics->CreateRigidDynamic(goTransform->position);

	if (collider_info != nullptr)
		rigid_dynamic->attachShape(*collider_info->colliderShape);

	name = "Rigidbody";

	use_gravity = true;
	use_kinematic = false;

	lock_linearX = false, lock_linearY = false, lock_linearZ = false;
	lock_angularX = false, lock_angularY = false, lock_angularZ = false;

	mass = 10.0f;
	density = 1.0f;

	force = float3::zero;
	torque = float3::zero;

	linear_vel = float3::zero;
	angular_vel = float3::zero;

	linear_damping = 0.0f;
	angular_damping = 0.0f;

	EnableGravity(use_gravity);
	EnableKinematic(use_kinematic);

	SetMass(mass);
	SetDensity(density);

	AddForce(force);
	AddTorque(torque);

	SetLinearVelocity(linear_vel);
	SetAngularVelocity(angular_vel);

	SetLinearDamping(linear_damping);
	SetAngularDamping(angular_damping);
}

C_RigidBody::~C_RigidBody()
{
	EngineExternal->modulePhysics->ReleaseActor((physx::PxRigidActor*)rigid_dynamic);
}

void C_RigidBody::Update()
{
	//Just update transform if we have rigidbody simulation
	//if (App->timeManager->started) {

		//if (collider_info != nullptr) {

		//	goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });
		//	Quat rot = { rigid_dynamic->getGlobalPose().q.x, rigid_dynamic->getGlobalPose().q.y, rigid_dynamic->getGlobalPose().q.z,  rigid_dynamic->getGlobalPose().q.w };
		//	float3 new_rot = rot.ToEulerXYZ() * RADTODEG;

		//	goTransform->SetRotation({ new_rot.x,new_rot.y, new_rot.z, });
		//}
		//else
		//	goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });

		//goTransform->UpdateNodeTransforms();

	//}
	//else {
	//	if (collider_info != nullptr)
	//		rigid_dynamic->setGlobalPose(PxTransform({ collider_info->transform->position.x, collider_info->transform->position.y, collider_info->transform->position.z }));
	//	else
	//		goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });
	//}

	//TODO: MOVE RIGID BODY IF GLOBAL POSITION CHANGED
}

#ifndef STANDALONE
bool C_RigidBody::OnEditor()
{
	if (Component::OnEditor() == true)
	{

	
		return true;
	}
	return false;
}
#endif // !STANDALONE

