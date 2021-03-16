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
//#include "CO_ParticleSystem.h"
//#include "Application.h"
//#include "MO_Camera3D.h"

C_Billboard::C_Billboard(GameObject* _gm):Component(_gm), currentAlignment(BILLBOARD_ALIGNMENT::SCREEN_ALIGNED)
{
	name = "Billboard";

	/*res_mesh = App->resources->GetBillboard();
	res_texture = nullptr;
	transform = new C_Transform(Component::Type::Billboard);*/
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
	UseAlignment();

	//if the gameObject has particle system
	if (gameObject->GetComponent(Component::TYPE::PARTICLE_SYSTEM) != nullptr) {
		//TODO: Check if the particle system has material

		/*if(gameObject->GetComponent(Component::TYPE::PARTICLE_SYSTEM) != nullptr)
		{
			glColor4f(color.r, color.g, color.b, color.a);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


			if (owner->particle_system->particle_material->GetResourceTexture() != nullptr && owner->particle_system->particle_material->HasTexture())
				glBindTexture(GL_TEXTURE_2D, owner->particle_system->particle_material->GetTexID());
			//else
				//glColor4f(mat->color.r, mat->color.g, mat->color.b, mat->color.a);
		}*/
	}

	/*
	transform->RecalculateMatrix();

	glPushMatrix();
	glMultMatrixf(transform->localMatrix.Transposed().ptr());

	res_mesh->Draw();

	glPopMatrix();
	*/
}

Quat C_Billboard::GetAlignment()
{
	switch (currentAlignment)
	{
	case BILLBOARD_ALIGNMENT::SCREEN_ALIGNED:
		return ScreenAlign();
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

	return Quat::identity;
}

void C_Billboard::UseAlignment()
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

Quat C_Billboard::ScreenAlign()
{
	//Option 1: Extract the rotation from the view matrix

	/*float3 normal = (EngineExternal->moduleRenderer3D->activeRenderCamera->GetPosition() - this->transform->position).Normalized();
	
	float3 up = App->camera->camera->frustum.up;
	float3 right = normal.Cross(up);

	float3x3 mat = float3x3::identity;
	mat.Set(-right.x, -right.y, -right.z, up.x, up.y, up.z, normal.x, normal.y, normal.z);

	transform->rotation = mat.Inverted().ToQuat();*/

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

void C_Billboard::WorldAlign()
{
	//The way is to generate a rotation matrix that +z looks at the camera
}

void C_Billboard::AxisAlign()
{
	//Use the global y-axis
}
