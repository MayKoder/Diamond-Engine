#include "C_Transform.h"
#include "ImGui/imgui.h"
#include"ImGui/imgui_internal.h"
#include "Globals.h"
#include <vector>
#include"GameObject.h"

C_Transform::C_Transform() : Component(nullptr), updateTransform(false)
{
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, localScale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTRANS = globalTransform.Transposed();

	name = "Transform";
}

C_Transform::C_Transform(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/): Component(_gm), updateTransform(false)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, localScale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTRANS = globalTransform.Transposed();

	name = "Transform";
}

C_Transform::~C_Transform()
{
}

void C_Transform::Update()
{
	if (updateTransform)
		UpdateTransform();
}

bool C_Transform::OnEditor()
{
	if (Component::OnEditor() == true)
	{

		int offset = ImGui::CalcTextSize("Local Position: ").x + 16;
		ImGui::Text("Local Position: "); 
		ImGui::SameLine(); 
		if (ImGui::DragFloat3("##lPos", &position[0], 0.1f))
			updateTransform = true;


		ImGui::Text("Rotation: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat3("##lRot", &eulerRotation[0], 0.1f))
			updateTransform = true;


		ImGui::Text("Scale: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat3("##lScale", &localScale[0], 0.1f))
			updateTransform = true;

		if (ImGui::Button("Reset Transform"))
			ResetTransform();

		// GLOBAL MATRIX //
		ImGui::Separator();
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


		ImGui::Text("Scale: ");
		ImGui::SameLine();
		ImGui::Text("%f, %f, %f", globalTransform.GetScale().x, globalTransform.GetScale().y, globalTransform.GetScale().z);

		return true;
	}
	return false;
}

//TODO: This should be recursive
void C_Transform::UpdateTransform()
{
	std::vector<C_Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	rotation = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);
	localTransform = float4x4::FromTRS(position, rotation, localScale);

	if (!transformsToUpdate.empty()) 
	{
		for (size_t i = 0; i < transformsToUpdate.size(); i++)
		{
			if (transformsToUpdate[i]->gameObject->parent != nullptr) 
			{
				C_Transform* parentTra = transformsToUpdate[i]->gameObject->parent->transform;

				if (parentTra != nullptr) {
					transformsToUpdate[i]->globalTransform = parentTra->globalTransform * transformsToUpdate[i]->localTransform;// = global = global parent * local
					transformsToUpdate[i]->globalTransformTRANS = transformsToUpdate[i]->globalTransform.Transposed();
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


void C_Transform::SetTransformMatrix(float3 _position, Quat _rotation, float3 _localScale)
{
	position = _position;
	rotation = _rotation;
	localScale = _localScale;

	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	localTransform = float4x4::FromTRS(position, rotation, localScale);

	if (gameObject->parent->transform != nullptr) {
		globalTransform = gameObject->parent->transform->globalTransform * localTransform;
		globalTransformTRANS = globalTransform.Transposed();
	}
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