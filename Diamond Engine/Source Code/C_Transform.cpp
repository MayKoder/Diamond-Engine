#include "C_Transform.h"
#include"ImGui/imgui.h"

C_Transform::C_Transform(GameObject* _gm): Component(_gm)
{
	transform.SetIdentity();
}

C_Transform::~C_Transform()
{
}

void C_Transform::Update()
{
}

void C_Transform::OnEditor()
{
	if (ImGui::CollapsingHeader("Transform")) 
	{
		ImGui::Text("Position: "); ImGui::SameLine(); ImGui::Text("%f, %f, %f", transform.Col3(3).x, transform.Col3(3).y, transform.Col3(3).z);
	}
}
