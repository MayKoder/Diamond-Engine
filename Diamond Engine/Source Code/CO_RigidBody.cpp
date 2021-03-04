#include"DETime.h"

#include "CO_RigidBody.h"
#include "CO_Collider.h"
#include "CO_Transform.h"
#include "CO_MeshRenderer.h"

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
	goTransform = dynamic_cast<C_Transform*>(_gm->GetComponent(Component::TYPE::TRANSFORM));
	collider_info = dynamic_cast<C_Collider*>(_gm->GetComponent(Component::TYPE::Collider));
	mesh = dynamic_cast<C_MeshRenderer*>(_gm->GetComponent(Component::TYPE::MESH_RENDERER));

	Quat rot;
	float3 pos, scale;
	goTransform->globalTransform.Decompose(pos, rot, scale);
	if(mesh != nullptr)
	pos = mesh->globalOBB.pos;



	

	rigid_dynamic = EngineExternal->modulePhysics->CreateRigidDynamic(pos, rot);

	float4x4 worldtrans = goTransform->globalTransform;
	float4x4 pivotrans = float4x4::FromTRS(pos, rot, scale);	
	//	pivotrans = global_to_pivot * worldtrans;
	global_to_pivot =  worldtrans.Inverted() * pivotrans;

	if (collider_info != nullptr)
	{
		rigid_dynamic->attachShape(*collider_info->colliderShape);
		collider_info->rigidbody = this;
	}

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

	rigid_dynamic->setRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z | physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);

	rigid_dynamic->userData = this->gameObject;

}

C_RigidBody::~C_RigidBody()
{

	if (collider_info != nullptr)
		collider_info->rigidbody = nullptr;
	//rigid_dynamic->getGlobalPose();
	EngineExternal->modulePhysics->ReleaseActor((physx::PxRigidActor*)rigid_dynamic);
}

void C_RigidBody::PostUpdate()
{
	//Just update transform if we have rigidbody simulation
	//if (App->timeManager->started) {
	
	
		
		Quat rot;
		float3 pos, scale;
		goTransform->globalTransform.Decompose(pos, rot, scale);
		//pos = mesh->globalOBB.pos;
		

		float4x4 pivotrans = float4x4::FromTRS(pos, rot, scale);

		float4x4 worldtrans = pivotrans * global_to_pivot;
		worldtrans.Decompose(pos, rot, scale);
		physx::PxQuat rotation = { rot.x,  rot.y, rot.z, rot.w };
		rigid_dynamic->setGlobalPose(physx::PxTransform({ pos.x, pos.y, pos.z }, rotation));

	
		
	/*}
	else {
		if (collider_info != nullptr)
			rigid_dynamic->setGlobalPose(PxTransform({ collider_info->transform->position.x, collider_info->transform->position.y, collider_info->transform->position.z }));
		else
			goTransform->SetPosition({ rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z });
	}*/

	//TODO: MOVE RIGID BODY IF GLOBAL POSITION CHANGED
}

void C_RigidBody::Step()
{
	if (DETime::state == GameState::PLAY)
	{
		float4x4 worldtrans = goTransform->globalTransform;

		float3 pos, scale;
		Quat rot;
		worldtrans.Decompose(pos, rot, scale);
		pos = { rigid_dynamic->getGlobalPose().p.x, rigid_dynamic->getGlobalPose().p.y, rigid_dynamic->getGlobalPose().p.z };
		rot = { rigid_dynamic->getGlobalPose().q.x, rigid_dynamic->getGlobalPose().q.y, rigid_dynamic->getGlobalPose().q.z,  rigid_dynamic->getGlobalPose().q.w };


		worldtrans = float4x4::FromTRS(pos, rot, scale);

		//	float4x4 pivotrans =  global_to_pivot.Inverted() * worldtrans;
		float4x4 pivotrans = worldtrans * global_to_pivot.Inverted();

		goTransform->SetTransformWithGlobal(pivotrans);


	}
}
void C_RigidBody::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	DEJson::WriteBool(nObj, "kinematic", use_kinematic);
	
}

void C_RigidBody::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	//if (nObj.ReadBool("IsEmpty") == true)
	//	return;

	bool temp = use_kinematic;
	use_kinematic = nObj.ReadBool("kinematic");
	if (temp != use_kinematic)
		EnableKinematic(use_kinematic);
}

#ifndef STANDALONE
bool C_RigidBody::OnEditor()
{
	if (Component::OnEditor() == true)
	{

		bool temp = use_kinematic;

		ImGui::Checkbox("Is Static", &use_kinematic);
		if (temp != use_kinematic)
			EnableKinematic(use_kinematic);
		
		return true;
	}
	return false;
}
#endif // !STANDALONE

