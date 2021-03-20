#include"Application.h"
#include "Globals.h"
#include"GameObject.h"

#include "MO_Physics.h"
#include "MO_Renderer3D.h"
#include"MO_Editor.h"

#include "CO_RigidBody.h"
#include "CO_Collider.h"
#include "CO_BoxCollider.h"
#include "CO_Transform.h"
#include "CO_MeshRenderer.h"

#include <vector>



C_BoxCollider::C_BoxCollider() : C_Collider(nullptr)
{

	name = "Box Collider_" + std::to_string(-1);
}


C_BoxCollider::C_BoxCollider(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/) : C_Collider(_gm)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{
	int indexNum = _gm != nullptr ?_gm->GetComponentsOfType(Component::TYPE::BOXCOLLIDER).size() + _gm->GetComponentsOfType(Component::TYPE::COLLIDER).size() : 0;
	name = "Box Collider_" + std::to_string(indexNum);
	isTrigger = false;
	shape = ColliderShape::CUBE;

	//Checks if component does have any owner and get additional data to be created


	transform = dynamic_cast<C_Transform*>(_gm->GetComponent(Component::TYPE::TRANSFORM));
	rigidbody = dynamic_cast<C_RigidBody*>(_gm->GetComponent(Component::TYPE::RIGIDBODY));
	mesh = dynamic_cast<C_MeshRenderer*>(_gm->GetComponent(Component::TYPE::MESH_RENDERER));



	//We first initialize material to create shape later
	colliderMaterial = EngineExternal->modulePhysics->CreateMaterial();
	localTransform = float4x4::identity;
	colliderShape = nullptr;

	//If gameObject does have mesh we apply measures directly to collider from OBB
	if (mesh != nullptr) {

		colliderSize = mesh->globalOBB.Size() / 2;
		if (colliderSize.y <= 0.0f) //I do this for plane meshes, but maybe we can remove this once we use mesh shapes
			colliderSize.y = 0.01f;
		//	colliderShape = App->physX->CreateCollider(type, colliderSize / 2, colliderMaterial);
		colliderShape = EngineExternal->modulePhysics->CreateBoxCollider(colliderSize, colliderMaterial);

	}
	else {
		colliderSize = { 0.5f, 0.5f, 0.5f };
		colliderShape = EngineExternal->modulePhysics->CreateBoxCollider(colliderSize, colliderMaterial);
	}

	/*colliderEuler = (transform->euler - owner->RotationOffset).Div(owner->SizeOffset);
	SetRotation(colliderEuler);*/


	//If we have a rigid body and doesnt have reference collider we attach the current one
	
	//if (rigidbody != nullptr)
	//	rigidbody->collider_info.push_back(this);

	//if (mesh != nullptr) {
	//	colliderPos = (mesh->globalOBB.pos);
	//	colliderPos.Set(0, 0, 0);
	//	SetPosition(colliderPos);
	//	
	//}
	//else {
	//	/*colliderPos = (transform->position - owner->PositionOffset).Div(owner->SizeOffset);
	//	SetPosition(colliderPos);
	//	owner->PositionOffset = float3::zero;
	//	owner->SizeOffset = float3::one;
	//	owner->RotationOffset = float3::zero;*/
	//}


	

	//We attach shape to a static or dynamic rigidbody to be collidable.
	if (rigidbody != nullptr && colliderShape != nullptr) {
		rigidbody->rigid_dynamic->attachShape(*colliderShape);
		rigidbody->collider_info.push_back(this);

	}
	//else {
	//	_gm->AddComponent(Component::TYPE::RIGIDBODY);
	//	rigidbody = dynamic_cast<C_RigidBody*>(_gm->GetComponent(Component::TYPE::RIGIDBODY));
	//	rigidbody->use_kinematic = true;
	//	rigidbody->EnableKinematic(rigidbody->use_kinematic);
	//	rigidbody->rigid_dynamic->attachShape(*colliderShape);
	//	rigidbody->collider_info.push_back(this);

	//	//	rigidbody = dynamic_cast<C_RigidBody*>(_gm->AddComponent(Component::Type::RigidBody));

	////	rigidbody->rigid_dynamic->attachShape(*colliderShape);

	//}



	colliderShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

}

C_BoxCollider::~C_BoxCollider()
{
	LOG(LogType::L_NORMAL, "Deleting Box Collider");

	if (colliderMaterial != nullptr)
		colliderMaterial->release();
	rigidbody = dynamic_cast<C_RigidBody*>(gameObject->GetComponent(Component::TYPE::RIGIDBODY));

	if (rigidbody != nullptr)
	{
		rigidbody->rigid_dynamic->detachShape(*colliderShape);
		for (int i = 0; i < rigidbody->collider_info.size(); i++)
		{
			if (rigidbody->collider_info[i] == this)
			{
				rigidbody->collider_info.erase(rigidbody->collider_info.begin() + i);
				i--;
			}
			
		}
	}

	if (colliderShape != nullptr)
		colliderShape->release();

}

void C_BoxCollider::Update()
{
#ifndef STANDALONE

	if(rigidbody == nullptr)
		rigidbody = dynamic_cast<C_RigidBody*>(gameObject->GetComponent(Component::TYPE::RIGIDBODY));

	if (colliderShape != nullptr)
	{
		//EngineExternal->modulePhysics->DrawCollider(this);

		float4x4 trans;
		if (rigidbody != nullptr && rigidbody->rigid_dynamic)
			trans = EngineExternal->modulePhysics->PhysXTransformToF4F(rigidbody->rigid_dynamic->getGlobalPose());
		else
			trans = transform->globalTransform;

		trans = trans * localTransform;
		//SetPosition(pos);
		//trans = EngineExternal->modulePhysics->PhysXTransformToF4F(colliderShape->getLocalPose());

		physx::PxBoxGeometry boxCollider;



		colliderShape->getBoxGeometry(boxCollider);

		float3 half_size = { boxCollider.halfExtents.x, boxCollider.halfExtents.y, boxCollider.halfExtents.z };

			glPushMatrix();
			glMultMatrixf(trans.Transposed().ptr());


			glLineWidth(2.0f);
			glColor3f(0.0f, 1.0f, 0.0f);
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

}





void C_BoxCollider::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	//	DEJson::WriteBool(nObj, "kinematic", use_kinematic);
	float3 pos, scale;
	Quat rot;
	localTransform.Decompose(pos, rot, scale);

	Component::SaveData(nObj);

	DEJson::WriteBool(nObj, "isTrigger", isTrigger);

	DEJson::WriteVector3(nObj, "Position", pos.ptr());
	DEJson::WriteQuat(nObj, "Rotation", rot.ptr());
	DEJson::WriteVector3(nObj, "Scale", scale.ptr());

}

void C_BoxCollider::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);
	float3 pos, scale;
	Quat rot;
	bool trigger;

	trigger = nObj.ReadBool("isTrigger");
	if (trigger != isTrigger)
	{
		SetTrigger(trigger);
		isTrigger = trigger;
	}

	pos = nObj.ReadVector3("Position");
	rot = nObj.ReadQuat("Rotation");
	scale = nObj.ReadVector3("Scale");


	physx::PxTransform physTrans;
	physTrans.p = physx::PxVec3(pos.x, pos.y, pos.z);
	physTrans.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);
	float3 newSize;
	newSize.x = colliderSize.x * scale.x;
	newSize.y = colliderSize.y * scale.y;
	newSize.z = colliderSize.z * scale.z;
	colliderShape->setGeometry(physx::PxBoxGeometry(newSize.x, newSize.y, newSize.z));
	colliderShape->setLocalPose(physTrans);

	localTransform = float4x4::FromTRS(pos, rot, scale);

}

