#include "CO_Transform2D.h"

#include "GameObject.h"

#include "Application.h"
#include "MO_GUI.h"

#include "MathGeoLib/include/Math/float4x4.h"
#include "ImGui/imgui.h"

C_Transform2D::C_Transform2D(GameObject* gameObject) : Component(gameObject),
	rotation(0.0f),
	localRotation(0.0f),
	updateTransform(false)
{
	name = "Transform 2D";

	memset(position, 0.f, sizeof(float) * 2);
	memset(localPos, 0.f, sizeof(float) * 2);

	size[0] = 20.f;
	size[1] = 20.f;
}


C_Transform2D::~C_Transform2D()
{

}


void C_Transform2D::Update()
{
	if (updateTransform == true)
		UpdateTransform();
}


#ifndef STANDALONE
bool C_Transform2D::OnEditor()
{
	//TODO Add modify pos logic
	if (Component::OnEditor() == true)
	{
		int offset = ImGui::CalcTextSize("Position: ").x + 16;
		ImGui::Text("Position: ");
		ImGui::SameLine();
		if (ImGui::DragFloat2("##lPosition", &localPos[0], 0.1f))
			updateTransform = true;


		ImGui::Text("Size: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat2("##lSize", &size[0], 0.1f))
			updateTransform = true;

		ImGui::Text("2D Rotation: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat("##lRotation", &localRotation, 0.1f))
			updateTransform = true;
	}


	return true;
}
#endif // !STANDALONE


float4x4 C_Transform2D::GetGlobal2DTransform()
{
	float positionX = position[0] / 100.f;
	float positionY = position[1] / 100.f;

	float scaleX = size[0] / 100.f;
	float scaleY = size[1] / 100.f;

	return float4x4::FromTRS(float3(positionX, positionY, 0), Quat::FromEulerXYZ(0, 0, rotation), float3(scaleX, scaleY, 1)).Transposed();
}


void C_Transform2D::SaveData(JSON_Object* nObj)
{

}


void C_Transform2D::LoadData(DEConfig& nObj)
{

}


void C_Transform2D::UpdateTransform()
{
	updateTransform = false;

	if (gameObject->parent != nullptr)
	{
		Component* parentTrans = gameObject->parent->GetComponent(Component::TYPE::TRANSFORM_2D);
		if (parentTrans != nullptr)
		{
			C_Transform2D* parentTransform = static_cast<C_Transform2D*>(parentTrans);
			position[0] = localPos[0] + parentTransform->position[0];
			position[1] = localPos[1] + parentTransform->position[1];
			rotation = localRotation + parentTransform->rotation;
		}

		else
		{
			position[0] = localPos[0];
			position[1] = localPos[1];
			rotation = localRotation;
		}
	}

	int childCount = gameObject->children.size();
	for (int i = 0; i < childCount; ++i)
	{
		Component* childComponent = gameObject->children[i]->GetComponent(Component::TYPE::TRANSFORM_2D);

		if (childComponent != nullptr)
		{
			C_Transform2D* childTransform = (C_Transform2D*)childComponent;
			childTransform->UpdateTransform();
		}
	}
}


void C_Transform2D::SetTransform(float locPosX, float locPosY, float locRotation)
{
	localPos[0] = locPosX;
	localPos[1] = locPosY;
	localRotation = locRotation;

	if (gameObject->parent != nullptr)
	{
		Component* parentTransformComp = gameObject->parent->GetComponent(Component::TYPE::TRANSFORM_2D);
		if (parentTransformComp != nullptr)
		{
			C_Transform2D* parentTransform = (C_Transform2D*)parentTransformComp;

			position[0] = localPos[0] + parentTransform->position[0];
			position[1] = localPos[1] + parentTransform->position[1];
			rotation = localRotation + parentTransform->rotation;
		}
	}
}