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

C_Billboard::C_Billboard(GameObject* _gm):Component(_gm), 
currentAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED), 
horizontalAligned(true),
transf(nullptr)
{
	name = "Billboard";
	transf= static_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM));
}

C_Billboard::~C_Billboard()
{
	transf = nullptr;
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
	case BILLBOARD_ALIGNMENT::CAMERA_ALIGNED: tempAlignment = "Camera Aligned"; break;
	case BILLBOARD_ALIGNMENT::WORLD_ALIGNED: tempAlignment = "World Aligned"; break;
	}
	ImGui::Text("Current Billboard: %s", tempAlignment.c_str());

	if (ImGui::BeginMenu("Change billboard")) {
		if (ImGui::MenuItem("Screen Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED);
		if (ImGui::MenuItem("Camera Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::CAMERA_ALIGNED);
		if (ImGui::MenuItem("World Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::WORLD_ALIGNED);
		if (ImGui::MenuItem("Axis Aligned")) SetAlignment(BILLBOARD_ALIGNMENT::AXIS_ALIGNED);

		ImGui::EndMenu();
	}

	if (currentAlignment == BILLBOARD_ALIGNMENT::AXIS_ALIGNED) {
		std::string tempName = (horizontalAligned ? "Horizontal" : "Vertical");
		ImGui::Text("Current Aligned Axis: %s", tempName.c_str());
		if (ImGui::BeginMenu("Change aligned axis")) {
			if (ImGui::MenuItem("Horizontal")) horizontalAligned = true;
			if (ImGui::MenuItem("Vertical")) horizontalAligned = false;

			ImGui::EndMenu();
		}
	}

	return true;
}
#endif // !STANDALONE

void C_Billboard::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteInt(nObj, "billboardAlignment", (int)currentAlignment);
	DEJson::WriteBool(nObj, "horizantalAligned", horizontalAligned);
}


void C_Billboard::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);
	currentAlignment = (BILLBOARD_ALIGNMENT)nObj.ReadInt("billboardAlignment");
	horizontalAligned = nObj.ReadBool("horizantalAligned");
}

void C_Billboard::SetAlignment(BILLBOARD_ALIGNMENT new_alignment)
{
	currentAlignment = new_alignment;
}


Quat C_Billboard::GetAlignment()
{
	switch (currentAlignment)
	{
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED:
		return ScreenAlign();
		break;
	case BILLBOARD_ALIGNMENT::CAMERA_ALIGNED:
		return CameraAlign();
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

	Frustum camFrust = EngineExternal->moduleRenderer3D->activeRenderCamera->camFrustrum;
	float3 billboardForward = -camFrust.front;

	float3 up = camFrust.up;
	float3 right = up.Cross(billboardForward);

	float3x3 mat = float3x3::identity;
	mat.Set(right.x, right.y, right.z, up.x, up.y, up.z, billboardForward.x, billboardForward.y, billboardForward.z);

	Quat ret = mat.Inverted().ToQuat();

	return ret;
}

Quat C_Billboard::CameraAlign()
{
	//Extract the rotation from the view matrix
	Frustum camFrust = EngineExternal->moduleRenderer3D->activeRenderCamera->camFrustrum;
	float3 billboardForward = (camFrust.pos-transf->globalTransform.TranslatePart()).Normalized();

	float3 up = camFrust.up;
	float3 right = up.Cross(billboardForward);
	up = billboardForward.Cross(right);

	float3x3 mat = float3x3::identity;
	mat.Set(right.x, right.y, right.z, up.x, up.y, up.z, billboardForward.x, billboardForward.y, billboardForward.z);

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
	if(horizontalAligned)
		return Quat(0.701, 0, 0, -0.713);
	else
		return Quat::identity;
}