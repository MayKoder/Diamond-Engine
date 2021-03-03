#include "CO_Transform.h"
#include "ImGui/imgui.h"
#include"ImGui/imgui_internal.h"
#include "Globals.h"
#include <vector>
#include"GameObject.h"

#include"CO_MeshRenderer.h"
#include"RE_Mesh.h"

#include "Application.h"
#include "MO_Editor.h"
#include"MO_Input.h"
#include "COMM_Transform.h"

C_Transform::C_Transform() : Component(nullptr), updateTransform(false), sendCommand(false)
{
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, localScale);
	rotation.Normalize();

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTRANS = globalTransform.Transposed();

	oldTransform.SetIdentity();

	name = "Transform";
}

C_Transform::C_Transform(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/): Component(_gm), updateTransform(false), sendCommand(false)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, localScale);
	rotation.Normalize();

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTRANS = globalTransform.Transposed();

	oldTransform.SetIdentity();

	name = "Transform";
}

C_Transform::~C_Transform()
{
}

void C_Transform::PostUpdate()
{
	if (updateTransform)
		UpdateTransform();
}

#ifndef STANDALONE
bool C_Transform::OnEditor()
{
	if (Component::OnEditor() == true)
	{

		int offset = ImGui::CalcTextSize("Local Position: ").x + 16;
		ImGui::Text("Local Position: "); 
		ImGui::SameLine(); 
		if (ImGui::DragFloat3("##lPos", &position[0], 0.1f))
			updateTransform = sendCommand = true;
		if (ImGui::IsItemClicked())
			oldTransform = GetCurrentGlobalMatrix();


		ImGui::Text("Rotation: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat3("##lRot", &eulerRotation[0], 0.1f))
			updateTransform = sendCommand = true;
		if (ImGui::IsItemClicked())
			oldTransform = GetCurrentGlobalMatrix();


		ImGui::Text("Scale: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat3("##lScale", &localScale[0], 0.1f))
			updateTransform = sendCommand = true;
		if (ImGui::IsItemClicked())
			oldTransform = GetCurrentGlobalMatrix();

		if (ImGui::Button("Reset Transform"))
			ResetTransform();
		if (ImGui::IsItemClicked())
			oldTransform = GetCurrentGlobalMatrix();

		//TODO: Doubli-click + enter input does not work as a command and won't we added to the shortcut manager
		if (EngineExternal->moduleInput->GetMouseButton(1) == KEY_STATE::KEY_UP && sendCommand == true)
		{
			EngineExternal->moduleEditor->shortcutManager.PushCommand(new COMM_Transform(gameObject->UID, GetCurrentGlobalMatrix().ptr(), oldTransform.ptr()));
			sendCommand = false;
		}

		// GLOBAL MATRIX //
		/*ImGui::Separator();
		ImGui::Text("Global Position: ");
		ImGui::SameLine();
		ImGui::PushItemWidth(50);
		ImGui::Text("%.2f", globalTransform.ptr()[3]);
		ImGui::SameLine();
		ImGui::Text("%.2f", globalTransform.ptr()[7]);
		ImGui::SameLine();
		ImGui::Text("%.2f", globalTransform.ptr()[11]);
		ImGui::PopItemWidth();

		ImGui::Text("Rotation: ");
		ImGui::SameLine();
		ImGui::Text("%f, %f, %f", globalTransform.ToEulerXYZ().x, globalTransform.ToEulerXYZ().y, globalTransform.ToEulerXYZ().z);

		ImGui::Text("Rotation Quaternion: ");
		ImGui::SameLine();
		Quat glQuat = globalTransform.RotatePart().ToQuat().Normalized();
		ImGui::Text("%f, %f, %f, %f", glQuat.x, glQuat.y, glQuat.z, glQuat.w);

		ImGui::Text("Scale: ");
		ImGui::SameLine();
		ImGui::Text("%f, %f, %f", globalTransform.GetScale().x, globalTransform.GetScale().y, globalTransform.GetScale().z);*/

		return true;
	}
	return false;
}
float4x4 C_Transform::GetCurrentGlobalMatrix()
{
	Quat rot = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);
	rot.Normalize();
	return gameObject->parent->transform->globalTransform * float4x4::FromTRS(position, rot, localScale);
}
#endif // !STANDALONE

//TODO: This should be recursive
void C_Transform::UpdateTransform()
{
	std::vector<C_Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	rotation = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);
	rotation.Normalize();

	localTransform = float4x4::FromTRS(position, rotation, localScale);


	if (!transformsToUpdate.empty()) 
	{
		for (size_t i = 0; i < transformsToUpdate.size(); i++)
		{
			if (transformsToUpdate[i]->gameObject->parent != nullptr) 
			{
				C_Transform* parentTra = transformsToUpdate[i]->gameObject->parent->transform;

				if (parentTra != nullptr) 
				{
					transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;// = global = global parent * local
					transformsToUpdate[i]->globalTransformTRANS = transformsToUpdate[i]->globalTransform.Transposed();

					//Update AABB and OBB
					transformsToUpdate[i]->UpdateBoxes();
				}
			}
		}
	}

	transformsToUpdate.clear();
	updateTransform = false;

}

//Populates an array of childs in descending order
C_Transform* C_Transform::GetRecursiveTransforms(C_Transform* node, std::vector<C_Transform*>& transforms)
{

	transforms.push_back(node);

	if (!node->gameObject->children.empty())
	{
		for (size_t i = 0; i < node->gameObject->children.size(); i++)
		{
			C_Transform* parTrans = node->gameObject->children[i]->transform;
			GetRecursiveTransforms(parTrans, transforms);
		}
	}

	return nullptr;
}

void C_Transform::UpdateBoxes()
{
	C_MeshRenderer* mesh = nullptr;
	mesh = dynamic_cast<C_MeshRenderer*>(gameObject->GetComponent(Component::TYPE::MESH_RENDERER));
	if (mesh != nullptr)
	{
		mesh->globalOBB = mesh->GetRenderMesh()->localAABB;
		mesh->globalOBB.Transform(globalTransform);

		// Generate global AABB
		mesh->globalAABB.SetNegativeInfinity();
		mesh->globalAABB.Enclose(mesh->globalOBB);
	}
}


void C_Transform::SetTransformMatrix(float3 _position, Quat _rotation, float3 _localScale)
{
	position = _position;
	rotation = _rotation.Normalized();
	localScale = _localScale;

	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	localTransform = float4x4::FromTRS(position, rotation, localScale);

	if (gameObject->parent->transform != nullptr) {
		globalTransform = gameObject->parent->transform->globalTransform * localTransform;
	}
	globalTransformTRANS = globalTransform.Transposed();
}

void C_Transform::SetTransformWithGlobal(float4x4& globalMat)
{
	//Same as we do with the reparenting
	globalTransform = globalMat;
	localTransform = gameObject->parent->transform->globalTransform.Inverted() * globalTransform;

	//Quat _rot;
	//float3 scale, pos;
	localTransform.Decompose(position, rotation, localScale);

	rotation.Normalize();
	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	globalTransformTRANS = globalTransform.Transposed();
	updateTransform = true;
}

const float* C_Transform::GetGlobalTransposed() const
{
	return globalTransformTRANS.ptr();
}

void C_Transform::ResetTransform()
{
	position = eulerRotation = float3::zero;
	localScale = float3::one;

	updateTransform = true;
}

float3 C_Transform::GetForward()
{
	return GetNormalizeAxis(2);
}

float3 C_Transform::GetUp()
{
	return GetNormalizeAxis(1);
}

float3 C_Transform::GetRight()
{
	return GetNormalizeAxis(0);
}

float3 C_Transform::GetNormalizeAxis(int i)
{
	return globalTransform.RotatePart().Col(i).Normalized();
}