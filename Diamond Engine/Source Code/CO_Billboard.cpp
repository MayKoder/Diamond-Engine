#include "CO_Billboard.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Application.h"
#include "MO_Renderer3D.h"
#include "CO_Camera.h"
#include "CO_Transform.h"

C_Billboard::C_Billboard(GameObject* _gm):Component(_gm), currentAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED)
{
	name = "Billboard";
}

C_Billboard::~C_Billboard()
{
}

#ifndef STANDALONE
bool C_Billboard::OnEditor()
{
	if(Component::OnEditor() == false)
		return false;

	ImGui::Separator();

	std::string tempAlignment = "test";
	switch (currentAlignment) {
	case BILLBOARD_ALIGNMENT::AXIS_ALIGNED: tempAlignment = "Axis Aligned"; break;
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED: tempAlignment = "Screen Aligned"; break;
	case BILLBOARD_ALIGNMENT::WORLD_ALIGNED: tempAlignment = "World Aligned"; break;
	}
	ImGui::Text("Current Billboard: %s", tempAlignment.c_str());

	if (ImGui::BeginMenu("Change billboard")) {
		if (ImGui::MenuItem("Screen Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED);
		if (ImGui::MenuItem("World Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::WORLD_ALIGNED);
		if (ImGui::MenuItem("Axis Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::AXIS_ALIGNED);

		ImGui::EndMenu();
	}

	return true;
}
#endif // !STANDALONE

void C_Billboard::SetAlignment(BILLBOARD_ALIGNMENT new_alignment)
{
	currentAlignment = new_alignment;
}

void C_Billboard::Draw()
{
	switch (currentAlignment)
	{
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED:
		ScreenAlign();
		break;
	case BILLBOARD_ALIGNMENT::WORLD_ALIGNED:
		WorldAlign();
		break;
	case BILLBOARD_ALIGNMENT::AXIS_ALIGNED:
		AxisAlign();
		break;
	default:
		break;
	}
}

Quat C_Billboard::GetAlignment()
{
	switch (currentAlignment)
	{
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED:
		return ScreenAlign();
		break;
	case BILLBOARD_ALIGNMENT::WORLD_ALIGNED:
		return WorldAlign();
		break;
	case BILLBOARD_ALIGNMENT::AXIS_ALIGNED:
		return AxisAlign();
		break;
	default:
		break;
	}

	return Quat::identity;
}

Quat C_Billboard::ScreenAlign()
{
	//Extract the rotation from the view matrix

	float3 cameraPos = EngineExternal->moduleRenderer3D->activeRenderCamera->GetPosition();
	C_Transform* transform = static_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM));
	float3 normal = (cameraPos - transform->position).Normalized();

	float3 up = transform->GetUp();
	float3 right = normal.Cross(up);

	float3x3 mat = float3x3::identity;
	mat.Set(-right.x, -right.y, -right.z, up.x, up.y, up.z, normal.x, normal.y, normal.z);

	Quat ret = mat.Inverted().ToQuat();

	return ret;
}

Quat C_Billboard::WorldAlign()
{
	//The way is to generate a rotation matrix that +z looks at the camera
	return Quat::identity;
}

Quat C_Billboard::AxisAlign()
{
	//Use the global y-axis
	return Quat::identity;
}