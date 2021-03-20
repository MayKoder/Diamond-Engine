#include"Application.h"
#include "Globals.h"
#include"GameObject.h"

#include "MO_Physics.h"
#include "MO_Renderer3D.h"
#include"MO_Editor.h"

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


}

C_Collider::~C_Collider()
{
	
}

void C_Collider::Update()
{

	
}

void C_Collider::SetPosition(float3 position) {

	colliderPos = (position);

	physx::PxTransform transformation = colliderShape->getLocalPose();
	float3 new_position = colliderPos;
	transformation.p = physx::PxVec3(new_position.x, new_position.y, new_position.z);

	colliderShape->setLocalPose(transformation); //Set new Transformation Values

}

void C_Collider::SetRotation(Quat rotation) {

	

	physx::PxTransform transformation = colliderShape->getLocalPose();
	transformation.q = physx::PxQuat(rotation.x, rotation.y, rotation.z, rotation.w);

	colliderShape->setLocalPose(transformation); //Set new Transformation Values

}

void C_Collider::SetScale(float3 scale, float radius) {

	float3 newSize;
	newSize.x = colliderSize.x * scale.x;
	newSize.y = colliderSize.y * scale.y;
	newSize.z = colliderSize.z * scale.z;

	if (newSize.x <= 0)
	{
		newSize.x = 0.1;
	}
	if (newSize.y <= 0)
	{
		newSize.y = 0.1;
	}
	if (newSize.z <= 0)
	{
		newSize.z = 0.1;
	}
	
	colliderShape->setGeometry(physx::PxBoxGeometry(newSize.x, newSize.y, newSize.z));

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
		colliderShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
		colliderShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);

	}
}

void C_Collider::UpdateValues() {

	SetPosition(mesh->globalOBB.pos);
//	SetRotation(owner->GetTransform()->euler);
	//float3 scale = owner->GetOBB().Size();
	//LOG("%f %f %f", scale.x, scale.y, scale.z);
	//SetScale();
}

void C_Collider::SaveData(JSON_Object* nObj)
{
//	Component::SaveData(nObj);
//
////	DEJson::WriteBool(nObj, "kinematic", use_kinematic);
//	float3 pos, scale;
//	Quat rot;
//	localTransform.Decompose(pos, rot, scale);
//
//	Component::SaveData(nObj);
//
//	DEJson::WriteBool(nObj, "isTrigger", isTrigger);
//
//	DEJson::WriteVector3(nObj, "Position", pos.ptr());
//	DEJson::WriteQuat(nObj, "Rotation", rot.ptr());
//	DEJson::WriteVector3(nObj, "Scale", scale.ptr());

}

void C_Collider::LoadData(DEConfig& nObj)
{
	/*Component::LoadData(nObj);
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

	localTransform = float4x4::FromTRS(pos, rot, scale);*/

}

#ifndef STANDALONE
bool C_Collider::OnEditor()
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
		if (ImGui::TreeNodeEx(std::string("Edit Collider: " + name).c_str(), node_flags))
		{


			ImGui::Columns(4, "Collidercolumns");
			ImGui::Separator();

			ImGui::Text("Position"); ImGui::Spacing(); ImGui::Spacing();// ImGui::NextColumn();
			ImGui::Text("Rotation"); ImGui::Spacing(); ImGui::Spacing(); //ImGui::NextColumn();
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
			//Rotation
			float r1 = rotation.x;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("  ", &r1);
			if (ImGui::IsItemActive())
			{
				float newrot = r1 - rotation.x;

				rotation.x = r1;
				float3 axis(1, 0, 0);
				Quat newquat = Quat::RotateAxisAngle(axis, newrot * pi / 180);
				rot = rot * newquat;
				localTransform = float4x4::FromTRS(pos, rot, scale);

				SetRotation(rot);
				
			}
			//Scale
			ImGui::SetNextItemWidth(50);
			float s1 = scale.x;

			ImGui::DragFloat("   ", &s1, 0.1f);
			if (ImGui::IsItemActive())
			{
				
				if (scale.x != s1)
				{
					//float newscale = s1 - scale.x;
					scale.x = s1;
					float3 size(scale.x, scale.y, scale.z);
					SetScale(size);
					if (scale.x <= 0)
					{
						scale.x = 0.1;
					}
				
					localTransform = float4x4::FromTRS(pos, rot, scale);
				}
					

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
			// Rotation
	
			float r2 = rotation.y;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("     ", &r2);
			if (ImGui::IsItemActive())
			{
				float newrot = r2 - rotation.y;

				rotation.y = r2;
				float3 axis(0, 1, 0);
				Quat newquat = Quat::RotateAxisAngle(axis, newrot * pi / 180);
				rot = rot * newquat;
				localTransform = float4x4::FromTRS(pos, rot, scale);

				SetRotation(rot);

			}
			//Scale
			float s2 = scale.y;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("      ", &s2, 0.1f);
			if (ImGui::IsItemActive())
			{
				if (scale.y != s2)
				{
					//float newscale = s1 - scale.x;
					scale.y = s2;
					float3 size(scale.x, scale.y, scale.z);
					SetScale(size);
					if (scale.y <= 0)
					{
						scale.y = 0.1;
					}
					localTransform = float4x4::FromTRS(pos, rot, scale);
				}
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
			// Rotation
			float r3 = rotation.z;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("        ", &r3);
			if (ImGui::IsItemActive())
			{
				float newrot = r3 - rotation.z;

				rotation.z = r3;
				float3 axis(0, 0, 1);
				Quat newquat = Quat::RotateAxisAngle(axis, newrot * pi / 180);
				rot = rot * newquat;
				localTransform = float4x4::FromTRS(pos, rot, scale);

				SetRotation(rot);

			}
			//Scale
			float s3 = scale.z;
			ImGui::SetNextItemWidth(50);
			ImGui::DragFloat("         ", &s3, 0.1f);
			if (ImGui::IsItemActive())
			{
				if (scale.z != s3)
				{
					//float newscale = s1 - scale.x;
					scale.z = s3;
					float3 size(scale.x, scale.y, scale.z);
					SetScale(size);
					if (scale.z <= 0)
					{
						scale.z = 0.1;
					}
					localTransform = float4x4::FromTRS(pos, rot, scale);
				}
			}
			ImGui::NextColumn();

			
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

