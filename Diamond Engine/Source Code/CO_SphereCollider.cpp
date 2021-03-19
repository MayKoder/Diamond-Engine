#include"Application.h"
#include "Globals.h"
#include"GameObject.h"

#include "MO_Physics.h"
#include "MO_Renderer3D.h"
#include"MO_Editor.h"

#include "CO_RigidBody.h"
#include "CO_Collider.h"
#include "CO_SphereCollider.h"
#include "CO_Transform.h"
#include "CO_MeshRenderer.h"

#include <vector>




C_SphereCollider::C_SphereCollider() : C_Collider(nullptr)
{
	name = "Box Collider";


}


C_SphereCollider::C_SphereCollider(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/) : C_Collider(_gm)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{

	name = "Sphere Collider";
	isTrigger = false;
	shape = ColliderShape::SPHERE;

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
		
		radius = 0;
		for (int i = 0; i < 3; i++)
		{
			if (radius < colliderSize[i])
				radius = colliderSize[i];
		}

		if (colliderSize.y <= 0.0f) 
			colliderSize.y = 0.01f;
		colliderShape = EngineExternal->modulePhysics->CreateSphereCollider(radius, colliderMaterial);

	}
	else {
		colliderSize = { 0.5f, 0.5f, 0.5f };
		radius = radius = 0.5;
		colliderShape = EngineExternal->modulePhysics->CreateSphereCollider(radius, colliderMaterial);
	}

	

	//We attach shape to a static or dynamic rigidbody to be collidable.
	if (rigidbody != nullptr && colliderShape != nullptr) {
		rigidbody->rigid_dynamic->attachShape(*colliderShape);
		rigidbody->collider_info.push_back(this);

	}
	

	colliderShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

}

C_SphereCollider::~C_SphereCollider()
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

void C_SphereCollider::Update()
{
#ifndef STANDALONE

	if (rigidbody == nullptr)
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
		float3 pos, scale;
		Quat rot;
		trans.Decompose(pos, rot, scale);
		scale.Set(radius, radius, radius);
		trans = trans.FromTRS(pos, rot, scale);
		//SetPosition(pos);
		//trans = EngineExternal->modulePhysics->PhysXTransformToF4F(colliderShape->getLocalPose());

		GLfloat x, y, z, alpha, beta; // Storage for coordinates and angles        
		int gradation = 10;

		glPushMatrix();
		glMultMatrixf(trans.Transposed().ptr());
		glLineWidth(2.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		for (alpha = 0.0; alpha < PI; alpha += PI / gradation)
		{
			
			
			glBegin(GL_LINE_STRIP);
			

			for (beta = 0.0; beta < 2.01 * PI; beta += PI / gradation)
			{
				x = cos(beta) * sin(alpha);
				y = sin(beta) * sin(alpha);
				z = cos(alpha);
				glVertex3f(x, y, z);
				x = cos(beta) * sin(alpha + PI / gradation);
				y = sin(beta) * sin(alpha + PI / gradation);
				z = cos(alpha + PI / gradation);
				glVertex3f(x, y, z);
			}

			glTranslatef(pos.x, 0.0f, 0.0f);
			glScalef(radius, radius, radius);
			glEnd();

			
		}
		glColor3f(1.0f, 1.0f, 1.0f);

		glPopMatrix();
	}
#endif // !STANDALONE

}





void C_SphereCollider::SaveData(JSON_Object* nObj)
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
	DEJson::WriteFloat(nObj, "Radius", radius);

}

void C_SphereCollider::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);
	float3 pos;
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
	radius = nObj.ReadFloat("Radius");


	physx::PxTransform physTrans;
	physTrans.p = physx::PxVec3(pos.x, pos.y, pos.z);
	physTrans.q = physx::PxQuat(rot.x, rot.y, rot.z, rot.w);
	float3 newSize;
	
	
	colliderShape->setGeometry(physx::PxSphereGeometry(radius));
	colliderShape->setLocalPose(physTrans);

	localTransform = float4x4::FromTRS(pos, rot, float3(1, 1, 1));

}


#ifndef STANDALONE
bool C_SphereCollider::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		bool trigger = isTrigger;
		ImGui::Checkbox("isTrigger", &trigger);

		if (trigger != isTrigger)
		{
			SetTrigger(trigger);
			isTrigger = trigger;
		}

		ImGui::Separator();
		if (ImGui::TreeNodeEx("Edit Collider", node_flags))
		{


			ImGui::Columns(4, "Collidercolumns");
			ImGui::Separator();

			ImGui::Text("Position"); ImGui::Spacing(); ImGui::Spacing();// ImGui::NextColumn();
		
			ImGui::Text("Scale"); ImGui::NextColumn();

			// Position
			float3 pos, scale;
			Quat rot;
			localTransform.Decompose(pos, rot, scale);
			float3 rotation = rot.ToEulerXYZ();

			float t = pos.x;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat(" ", &t);
			if (ImGui::IsItemActive())
			{
				pos.x = t;
				localTransform = float4x4::FromTRS(pos, rot, scale);

				SetPosition(pos);


			}
			ImGui::SetNextItemWidth(50);
			
			float rad = radius;
			ImGui::DragFloat("  ", &rad);
			if (ImGui::IsItemActive())
			{
				radius = rad;
				colliderShape->setGeometry(physx::PxSphereGeometry(radius));
			}
			ImGui::NextColumn();

			// Position

			float t1 = pos.y;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("    ", &t1);
			if (ImGui::IsItemActive())
			{
				pos.y = t1;
				localTransform = float4x4::FromTRS(pos, rot, scale);

				SetPosition(pos);
			}
		
		
			
			ImGui::NextColumn();

			// Position
			float t2 = pos.z;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("       ", &t2);
			if (ImGui::IsItemActive())
			{
				pos.z = t2;
				localTransform = float4x4::FromTRS(pos, rot, scale);

				SetPosition(pos);
			}
			
			ImGui::NextColumn();

			ImGui::Columns(1);
			ImGui::TreePop();

		}
		ImGui::Separator();

		//Editar materiales
		//colliderMaterial

		return true;
	}
	return false;
}
#endif // !STANDALONE



