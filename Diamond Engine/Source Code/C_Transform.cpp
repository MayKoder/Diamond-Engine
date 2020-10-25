#include "C_Transform.h"
#include"ImGui/imgui.h"
#include"Globals.h"

C_Transform::C_Transform(GameObject* _gm): Component(_gm)
{
	globalTransform.SetIdentity();
	localTransform.SetIdentity();
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
		ImGui::Text("Position: "); 
		ImGui::SameLine(); 
		ImGui::PushItemWidth(50);
		ImGui::DragFloat("X", &localTransform.ptr()[3]);
		ImGui::SameLine();
		ImGui::DragFloat("Y", &localTransform.ptr()[7]);
		ImGui::SameLine();
		ImGui::DragFloat("Z", &localTransform.ptr()[11]);
		ImGui::PopItemWidth();

		ImGui::Text("%f, %f, %f", localTransform.Col3(3).x, localTransform.Col3(3).y, localTransform.Col3(3).z);

		ImGui::Text("Rotation: ");
		ImGui::SameLine();
		ImGui::Text("%f, %f, %f", localTransform.ToEulerXYZ().x * RADTODEG, localTransform.ToEulerXYZ().y * RADTODEG, localTransform.ToEulerXYZ().z * RADTODEG);


		ImGui::Text("Scale: ");
		ImGui::SameLine();
		ImGui::Text("%f, %f, %f", localTransform.GetScale().x, localTransform.GetScale().y, localTransform.GetScale().z);
	}
}
