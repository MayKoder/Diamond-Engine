#include "C_Transform.h"
#include "ImGui/imgui.h"
#include"ImGui/imgui_internal.h"
#include "Globals.h"
#include <vector>
#include"GameObject.h"

C_Transform::C_Transform(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/): Component(_gm), updateTransform(false)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{
	globalTransform.SetIdentity();
	localTransform.SetIdentity();

	localTransform.Decompose(position, rotation, localScale);

	eulerRotation = rotation.ToEulerXYZ();

	globalTransformTRANS = globalTransform.Transposed();
}

C_Transform::~C_Transform()
{
}

void C_Transform::Update()
{
}

void C_Transform::OnEditor()
{
	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
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


		if (updateTransform) 
			UpdateTransform();


	}
}

void C_Transform::UpdateTransform()
{
	std::vector<C_Transform*> transformsToUpdate;
	GetRecursiveTransforms(this, transformsToUpdate);

	rotation = Quat::FromEulerXYZ(eulerRotation.x * DEGTORAD, eulerRotation.y * DEGTORAD, eulerRotation.z * DEGTORAD);
	localTransform = float4x4::FromTRS(position, rotation, localScale);

	//Not working
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

void C_Transform::SetTransformMatrix(float3 _position, Quat _rotation, float3 _localScale, C_Transform* parent)
{
	position = _position;
	rotation = _rotation;
	localScale = _localScale;

	eulerRotation = rotation.ToEulerXYZ() * RADTODEG;

	localTransform = float4x4::FromTRS(position, rotation, localScale);

	if (parent != nullptr) {
		globalTransform = parent->globalTransform * localTransform;
		globalTransformTRANS = globalTransform.Transposed();
	}
}

const float* C_Transform::GetGlobalTransposed() const
{
	return globalTransformTRANS.ptr();
}
