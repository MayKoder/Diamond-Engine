#include"Application.h"
#include "Globals.h"
#include"GameObject.h"

#include "MO_Physics.h"
#include "MO_Renderer3D.h"

#include "CO_RigidBody.h"
#include "CO_Collider.h"
#include "CO_Transform.h"
#include "CO_MeshRenderer.h"

#include <vector>



C_Collider::C_Collider() : Component(nullptr)
{
	name = "Collider";


}


C_Collider::C_Collider(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/) : Component(_gm)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{

	name = "Collider";
	isTrigger = false;

	//Checks if component does have any owner and get additional data to be created


		transform = dynamic_cast<C_Transform*>(_gm->GetComponent(Component::TYPE::TRANSFORM));
		rigidbody = dynamic_cast<C_RigidBody*>(_gm->GetComponent(Component::TYPE::RigidBody));
		mesh = dynamic_cast<C_MeshRenderer*>(_gm->GetComponent(Component::TYPE::MESH_RENDERER));

		

		//We first initialize material to create shape later
		colliderMaterial = EngineExternal->modulePhysics->CreateMaterial();


		//If gameObject does have mesh we apply measures directly to collider from OBB
		if (mesh != nullptr) {

			colliderSize = mesh->globalOBB.Size();
			if (colliderSize.y <= 0.0f) //I do this for plane meshes, but maybe we can remove this once we use mesh shapes
				colliderSize.y = 0.01f;
		//	colliderShape = App->physX->CreateCollider(type, colliderSize / 2, colliderMaterial);
			colliderShape = EngineExternal->modulePhysics->CreateCollider( colliderSize / 2, colliderMaterial);

		}
		else {
			colliderSize = { 1.0f, 1.0f, 1.0f };
			colliderShape = EngineExternal->modulePhysics->CreateCollider( colliderSize / 2, colliderMaterial);
		}

		/*colliderEuler = (transform->euler - owner->RotationOffset).Div(owner->SizeOffset);
		SetRotation(colliderEuler);*/


		//If we have a rigid body and doesnt have reference collider we attach the current one
		if (rigidbody != nullptr && rigidbody->collider_info == nullptr)
			rigidbody->collider_info = this;

		if (mesh != nullptr) {
			colliderPos = (mesh->globalOBB.pos);
			colliderPos.Set(0, 0, 0);
			SetPosition(colliderPos);
			
		}
		else {
			/*colliderPos = (transform->position - owner->PositionOffset).Div(owner->SizeOffset);
			SetPosition(colliderPos);
			owner->PositionOffset = float3::zero;
			owner->SizeOffset = float3::one;
			owner->RotationOffset = float3::zero;*/
		}


		rigidStatic = nullptr;

		//We attach shape to a static or dynamic rigidbody to be collidable.
		if (rigidbody != nullptr) {
			rigidbody->rigid_dynamic->attachShape(*colliderShape);
		}
		else {
			_gm->AddComponent(Component::TYPE::RigidBody);
			rigidbody = dynamic_cast<C_RigidBody*>(_gm->GetComponent(Component::TYPE::RigidBody));
			rigidbody->use_kinematic = true;
			rigidbody->EnableKinematic(rigidbody->use_kinematic);
			rigidbody->rigid_dynamic->attachShape(*colliderShape);

			
			//	rigidbody = dynamic_cast<C_RigidBody*>(_gm->AddComponent(Component::Type::RigidBody));

		//	rigidbody->rigid_dynamic->attachShape(*colliderShape);

		}

	

	
}

C_Collider::~C_Collider()
{
	if (rigidStatic != nullptr)
		EngineExternal->modulePhysics->ReleaseActor(dynamic_cast<physx::PxRigidActor*>(rigidStatic));

	if (colliderMaterial != nullptr)
		colliderMaterial->release();

	if (colliderShape != nullptr)
		colliderShape->release();
}

void C_Collider::Update()
{
#ifndef STANDALONE

	if (colliderShape != nullptr )
	{
		//EngineExternal->modulePhysics->DrawCollider(this);

		float4x4 trans;
		if (rigidbody != nullptr && rigidbody->rigid_dynamic)
		trans = EngineExternal->modulePhysics->PhysXTransformToF4F(rigidbody->rigid_dynamic->getGlobalPose());
		else
			trans = transform->globalTransform;


		//SetPosition(pos);
		//trans = EngineExternal->modulePhysics->PhysXTransformToF4F(colliderShape->getLocalPose());

		physx::PxBoxGeometry boxCollider;

		

		colliderShape->getBoxGeometry(boxCollider);

		float3 half_size = { boxCollider.halfExtents.x, boxCollider.halfExtents.y, boxCollider.halfExtents.z };
		
		glPushMatrix();
		glMultMatrixf(trans.Transposed().ptr());


		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-half_size.x, half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, half_size.z);
		glVertex3f(-half_size.x, half_size.y, half_size.z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(-half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, -half_size.y, half_size.z);
		glVertex3f(-half_size.x, -half_size.y, half_size.z);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(-half_size.x, half_size.y, -half_size.z);
		glVertex3f(-half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, -half_size.z);
		glVertex3f(half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, half_size.z);
		glVertex3f(half_size.x, -half_size.y, half_size.z);
		glVertex3f(-half_size.x, half_size.y, half_size.z);
		glVertex3f(-half_size.x, -half_size.y, half_size.z);
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glPopMatrix();
	}
	#endif // !STANDALONE
	
/*
	if (rigidStatic != nullptr) {
		float3 pos, scale;
		Quat rot;
		transform->globalTransform.Decompose(pos, rot, scale);

		physx::PxQuat rotation = { rot.x,  rot.y, rot.z, rot.w };
		pos = mesh->globalOBB.pos;
		rigidStatic->setGlobalPose(physx::PxTransform({ pos.x, pos.y, pos.z,  rotation }));
		

		float4x4 trans = EngineExternal->modulePhysics->PhysXTransformToF4F(rigidStatic->getGlobalPose());


		//SetPosition(pos);
		//trans = EngineExternal->modulePhysics->PhysXTransformToF4F(colliderShape->getLocalPose());

		physx::PxBoxGeometry boxCollider;



		colliderShape->getBoxGeometry(boxCollider);

		float3 half_size = { boxCollider.halfExtents.x, boxCollider.halfExtents.y, boxCollider.halfExtents.z };

		glPushMatrix();
		glMultMatrixf(trans.Transposed().ptr());


		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
		glVertex3f(-half_size.x, half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, half_size.z);
		glVertex3f(-half_size.x, half_size.y, half_size.z);
		glEnd();

		glBegin(GL_LINE_LOOP);
		glVertex3f(-half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, -half_size.y, half_size.z);
		glVertex3f(-half_size.x, -half_size.y, half_size.z);
		glEnd();

		glBegin(GL_LINES);
		glVertex3f(-half_size.x, half_size.y, -half_size.z);
		glVertex3f(-half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, -half_size.z);
		glVertex3f(half_size.x, -half_size.y, -half_size.z);
		glVertex3f(half_size.x, half_size.y, half_size.z);
		glVertex3f(half_size.x, -half_size.y, half_size.z);
		glVertex3f(-half_size.x, half_size.y, half_size.z);
		glVertex3f(-half_size.x, -half_size.y, half_size.z);
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glPopMatrix();
	}*/
}

void C_Collider::SetPosition(float3 position) {

	colliderPos = (position);

	physx::PxTransform transformation = colliderShape->getLocalPose();
	float3 new_position = colliderPos;
	transformation.p = physx::PxVec3(new_position.x, new_position.y, new_position.z);

	colliderShape->setLocalPose(transformation); //Set new Transformation Values

}

//void C_Collider::SetRotation(float3 rotation) {
//
//	colliderEuler = rotation;
//
//	PxTransform transformation = colliderShape->getLocalPose();
//	float3 rot = DEGTORAD * colliderEuler;
//	Quat new_rotation = Quat::FromEulerXYZ(rot.x, rot.y, rot.z);
//	transformation.q = PxQuat(new_rotation.x, new_rotation.y, new_rotation.z, new_rotation.w);
//
//	colliderShape->setLocalPose(transformation); //Set new Transformation Values
//
//}

void C_Collider::SetScale(float3 scale, float radius) {

	physx::PxTransform transformation = colliderShape->getLocalPose();
	float3 new_colliderDimensions = { 1.0f, 1.0f, 1.0f };
	float new_radius = 1.0f;

	transformation = SetLocalPose(transformation);
	colliderShape->setLocalPose(transformation);

	colliderSize = scale;
	new_colliderDimensions = colliderSize.Mul(transform->localScale) / 2;
	colliderShape->setGeometry(physx::PxBoxGeometry(new_colliderDimensions.x, new_colliderDimensions.y, new_colliderDimensions.z));

	/*switch (type)
	{
	case GeometryType::BOX:
		colliderSize = scale;
		new_colliderDimensions = colliderSize.Mul(transform->scale) / 2;
		colliderShape->setGeometry(PxBoxGeometry(new_colliderDimensions.x, new_colliderDimensions.y, new_colliderDimensions.z));
		break;
	case GeometryType::SPHERE:
		colliderRadius = radius;

		new_radius = colliderRadius;
		colliderShape->setGeometry(PxSphereGeometry(new_radius));
		break;
	case GeometryType::CAPSULE:
		colliderSize = scale;
		colliderRadius = radius;
		new_radius = colliderRadius;
		new_colliderDimensions = colliderSize.Mul(transform->scale) / 2;
		colliderShape->setGeometry(PxCapsuleGeometry(new_radius, new_colliderDimensions.y));

		break;
	}*/

}

physx::PxTransform C_Collider::SetLocalPose(physx::PxTransform transformation) {

	float3 new_position = colliderPos;
	float3 rot = DEGTORAD * colliderEuler;
	Quat new_rotation = Quat::FromEulerXYZ(rot.x, rot.y, rot.z);

	transformation.p = physx::PxVec3(new_position.x, new_position.y, new_position.z);
	transformation.q = physx::PxQuat(new_rotation.x, new_rotation.y, new_rotation.z, new_rotation.w);

	return transformation;

}

void C_Collider::SetTrigger(bool trigger) {

	if (trigger)
	{
		colliderShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
		colliderShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	else {
		colliderShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
		colliderShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
	}
}

void C_Collider::UpdateValues() {

	SetPosition(mesh->globalOBB.pos);
//	SetRotation(owner->GetTransform()->euler);
	//float3 scale = owner->GetOBB().Size();
	//LOG("%f %f %f", scale.x, scale.y, scale.z);
	//SetScale();
}
#ifndef STANDALONE
bool C_Collider::OnEditor()
{
	if (Component::OnEditor() == true)
	{


		return true;
	}
	return false;
}
#endif // !STANDALONE

