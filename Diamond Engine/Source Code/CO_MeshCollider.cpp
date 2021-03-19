#include"Application.h"
#include "Globals.h"
#include"GameObject.h"

#include "MO_Physics.h"
#include "MO_Renderer3D.h"
#include"MO_Editor.h"

#include "CO_RigidBody.h"
#include "CO_MeshCollider.h"
#include "CO_Transform.h"
#include "CO_MeshRenderer.h"

#include <vector>



C_MeshCollider::C_MeshCollider() : C_Collider(nullptr)
{
	name = "Mesh Collider";


}


C_MeshCollider::C_MeshCollider(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/) : C_Collider(_gm)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{

	name = "Mesh Collider";
	isTrigger = false;
	shape = ColliderShape::MESH;

	//Checks if component does have any owner and get additional data to be created


		transform = dynamic_cast<C_Transform*>(_gm->GetComponent(Component::TYPE::TRANSFORM));
		rigidbody = dynamic_cast<C_RigidBody*>(_gm->GetComponent(Component::TYPE::RIGIDBODY));
		mesh = dynamic_cast<C_MeshRenderer*>(_gm->GetComponent(Component::TYPE::MESH_RENDERER));


		//We first initialize material to create shape later
		colliderMaterial = EngineExternal->modulePhysics->CreateMaterial();
		localTransform = float4x4::identity;
	
		
		//We attach shape to a static or dynamic rigidbody to be collidable.
		colliderShape = nullptr;
		if (rigidbody != nullptr) {
			colliderShape = EngineExternal->modulePhysics->CreateMeshCollider(rigidbody->rigid_dynamic, _gm);
			rigidbody->rigid_dynamic->attachShape(*colliderShape);
			rigidbody->collider_info.push_back(this);
		}
	
	
		if(colliderShape != nullptr)
		colliderShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

}

C_MeshCollider::~C_MeshCollider()
{
	LOG(LogType::L_NORMAL, "Deleting Mesh Collider");

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

void C_MeshCollider::Update()
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

		trans = trans * localTransform;
		

		const physx::PxVec3* convexVerts = colliderShape->getGeometry().convexMesh().convexMesh->getVertices();
		float size = colliderShape->getGeometry().convexMesh().convexMesh->getNbVertices();
		const physx::PxU8* indexBuffer = colliderShape->getGeometry().convexMesh().convexMesh->getIndexBuffer();
		physx::PxU32 nbPolygons = colliderShape->getGeometry().convexMesh().convexMesh->getNbPolygons();

		
		glPushMatrix();
		glMultMatrixf(trans.Transposed().ptr());
		glLineWidth(2.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
	
		
		for (physx::PxU32 i = 0; i < nbPolygons; i++)
		{
			physx::PxHullPolygon face;
			bool status = colliderShape->getGeometry().convexMesh().convexMesh->getPolygonData(i, face);
			PX_ASSERT(status);
			PX_UNUSED(status);
			glBegin(GL_LINES);
			const physx::PxU8* faceIndices = indexBuffer + face.mIndexBase;
			for (physx::PxU32 j = 0; j < face.mNbVerts; j++)
			{
				physx::PxVec3 vec = convexVerts[faceIndices[j]];
				glVertex3f(vec.x, vec.y, vec.z);
				
			}
			glEnd();
	

		}
		
		glColor3f(1.0f, 1.0f, 1.0f);
		glPopMatrix();

	}
	#endif // !STANDALONE
	
}





void C_MeshCollider::SaveData(JSON_Object* nObj)
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

void C_MeshCollider::LoadData(DEConfig& nObj)
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

