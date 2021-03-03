#include "CO_Transform2D.h"

#include "GameObject.h"

#include "Application.h"
#include "MO_GUI.h"
#include "MO_Input.h"
#include "MO_Editor.h"

#include "COMM_Transform2D.h"

#include "MathGeoLib/include/Math/float4x4.h"
#include "ImGui/imgui.h"

C_Transform2D::C_Transform2D(GameObject* gameObject) : Component(gameObject),
	rotation(0.0f),
	localRotation(0.0f),
	updateTransform(false),
	send_command(false)
{
	name = "Transform 2D";

	memset(position, 0.f, sizeof(float) * 2);
	memset(localPos, 0.f, sizeof(float) * 2);

	size[0] = 20.f;
	size[1] = 20.f;

	SetPreviousParameters();
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
		if (ImGui::DragFloat2("##lPosition", &localPos[0], 0.1f)) {
			updateTransform = true;
			send_command = true;
		}
		if (ImGui::IsItemClicked()) 
			SetPreviousParameters();


		ImGui::Text("Size: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat2("##lSize", &size[0], 0.1f)) {
			updateTransform = true;
			send_command = true;
		}
		if (ImGui::IsItemClicked())
			SetPreviousParameters();


		ImGui::Text("2D Rotation: ");
		ImGui::SameLine();
		ImGui::SetCursorPosX(offset);
		if (ImGui::DragFloat("##lRotation", &localRotation, 0.1f)) {
			updateTransform = true;
			send_command = true;
		}if (ImGui::IsItemClicked())
			SetPreviousParameters();


		//TODO: Doubli-click + enter input does not work as a command and won't we added to the shortcut manager
		if (EngineExternal->moduleInput->GetMouseButton(1) == KEY_STATE::KEY_UP && send_command == true)
		{
			float next_param[5] = { localPos[0],localPos[1],localRotation,size[0],size[1] };
			EngineExternal->moduleEditor->shortcutManager.PushCommand(new COMM_Transform2D(gameObject->UID, next_param, previous_parameters));
			send_command = false;
		}
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
	Component::SaveData(nObj);

	DEJson::WriteVector2(nObj, "Position", &localPos[0]);
	DEJson::WriteFloat(nObj, "Rotation", localRotation);
	DEJson::WriteVector2(nObj, "Size", &size[0]);
}


void C_Transform2D::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	float2 pos = nObj.ReadVector2("Position");
	localRotation = nObj.ReadFloat("Rotation");
	float2 scl = nObj.ReadVector2("Size");

	localPos[0] = pos.x;
	localPos[1] = pos.y;

	size[0] = scl.x;
	size[1] = scl.y;

	updateTransform = true;
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

void C_Transform2D::SetPreviousParameters()
{
	previous_parameters[0] = localPos[0];
	previous_parameters[1] = localPos[1];
	previous_parameters[2] = localRotation;
	previous_parameters[3] = size[0];
	previous_parameters[4] = size[1];

}


void C_Transform2D::SetTransform(float locPosX, float locPosY, float locRotation, float sizeX, float sizeY)
{
	localPos[0] = locPosX;
	localPos[1] = locPosY;
	localRotation = locRotation;
	size[0] = sizeX;
	size[1] = sizeY;

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

void C_Transform2D::SetTrueUpdateTransform()
{
	updateTransform = true;
}
