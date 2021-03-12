#ifndef STANDALONE

#include "Globals.h"
#include "Application.h"
#include "MO_Camera3D.h"
#include "ImGui/imgui.h"
#include "MO_Input.h"
#include "MO_Window.h"
#include "MO_Editor.h"
#include "GameObject.h"

#include"CO_Transform.h"
#include"MathGeoLib/include/Math/float4.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled), mouseSensitivity(0.50f), cameraSpeed(40.f), cameraMovement(0.f, 0.f, 0.f),zoomLevel(20),reference(0.0f,0.0f,0.0f)
{
	editorCamera.camFrustrum.farPlaneDistance = 5000;
	editorCamera.camFrustrum.pos = float3(8.0f, 3.0f, 8.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG(LogType::L_NORMAL, "Setting up the camera");
	bool ret = true;
	CamZoom(0);

	reference = float3(0.0f, 0.0f, 0.0f);
	editorCamera.LookAt(float3(0.f, 0.f, 0.f));

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG(LogType::L_NORMAL, "Cleaning camera");

	return true;
}

#ifndef STANDALONE
void ModuleCamera3D::OnGUI()
{
	if (ImGui::CollapsingHeader("3D Input Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextWrapped("Camera Rotation Speed"); ImGui::SameLine(); ImGui::PushItemWidth(100.f); ImGui::DragFloat("##crs", &mouseSensitivity, 0.01f, 0.f, 999.f);
		ImGui::TextWrapped("Camera Movement Speed"); ImGui::SameLine(); ImGui::PushItemWidth(100.f); ImGui::DragFloat("##cms", &cameraSpeed, 0.01f, 0.f, 999.f);
	}
}
#endif // !STANDALONE

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	Move(cameraMovement);
	cameraMovement = float3::zero;

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::ProcessSceneKeyboard()
{
	cameraMovement.Set(0.f, 0.f, 0.f);
	const float dt = App->GetDT();

	float speed = cameraSpeed * dt;
	if (App->moduleInput->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2.f;

	if (App->moduleInput->GetKey(SDL_SCANCODE_B) == KEY_REPEAT) cameraMovement -= speed*editorCamera.camFrustrum.up;
	if (App->moduleInput->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT) cameraMovement += speed* editorCamera.camFrustrum.up;

	if (App->moduleInput->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) cameraMovement += editorCamera.camFrustrum.front * speed;
	if (App->moduleInput->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) cameraMovement -= editorCamera.camFrustrum.front * speed;


	if (App->moduleInput->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) cameraMovement -= editorCamera.camFrustrum.WorldRight() * speed;
	if (App->moduleInput->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) cameraMovement += editorCamera.camFrustrum.WorldRight() * speed;

	int mouseZ = App->moduleInput->GetMouseZ();
	if (mouseZ != 0)
	{
		//cameraMovement += editorCamera.camFrustrum.front * speed * App->moduleInput->GetMouseZ() * 250;
		CamZoom(mouseZ);
	}

	// Mouse motion ----------------
	
	//BUG: Will lock the mouse forever if the user docks two camera windows together lol
	//if (App->moduleInput->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN /*|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN*/)
	//{
	//	SDL_SetRelativeMouseMode(SDL_TRUE);
	//	SDL_WarpMouseInWindow(App->moduleWindow->window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	//}
	//else if (App->moduleInput->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_UP /*|| App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP*/)
	//{
	//	SDL_SetRelativeMouseMode(SDL_FALSE);
	//	SDL_WarpMouseInWindow(App->moduleWindow->window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	//}

	//ASK: Is this really the best way to rotate the camera? Maybe i should use a matrix
	//TODO: Camera rotation should not be affected by the program framerate
	if (App->moduleInput->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		FreeRotation(dt);
	}


	//Rotate around 0,0,0
	//ASK: Should i also include Right alt?
	//Maybe we could use quaternions?
	if (App->moduleInput->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->moduleInput->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) 
	{
		/*float3 target(0.f, 0.f, 0.f);
		if (App->moduleEditor->GetSelectedGO() != nullptr)
		{
			float3 maTogl = App->moduleEditor->GetSelectedGO()->transform->globalTransform.TranslatePart();
			target.Set(maTogl.x, maTogl.y, maTogl.z);
		}
		OrbitalRotation(target, dt);*/
		OrbitalRotation(dt);
	}


	if (App->moduleInput->GetKey(SDL_SCANCODE_F) == KEY_DOWN) 
	{
		float3 target(0.f, 0.f, 0.f);
		if (App->moduleEditor->GetSelectedGO() != nullptr) 
		{
			float3 maTogl = App->moduleEditor->GetSelectedGO()->transform->globalTransform.TranslatePart();
			target.Set(maTogl.x, maTogl.y, maTogl.z);
		}
		//FocusCamera(target, 10.f);
		MoveTo(target);
	}




	if (App->moduleInput->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
		PanCamera(dt);
}

void ModuleCamera3D::Move(const float3& movement)
{
	editorCamera.camFrustrum.pos += movement;
	reference += movement;
}

void ModuleCamera3D::MoveTo(const float3& destination)
{
	float3 movement;
	movement = destination - reference;
	Move(movement);
}

////Could be a good idea to use quaternions? Would it be faster?
////BUG: Camera will bug when the the horizontal rotation is almost 
////equal as the target X and Z coords
void ModuleCamera3D::OrbitalRotation(float dt)
{
	int dx = -App->moduleInput->GetMouseXMotion();
	int dy = -App->moduleInput->GetMouseYMotion();

	float distance = editorCamera.camFrustrum.pos.Distance(reference);

	Quat direction;
	editorCamera.camFrustrum.WorldMatrix().Decompose(float3(), direction, float3());

	//editorCamera.camFrustrum.pos = center;
	//ERROR, BUG, WARNING: CAMERA DOES A WEIRD FLIP WHEN ORBITATING PAST Y -1, Y 1, SHOULD INVERT WORLD X
	if (dy != 0)
	{
		float DeltaY = (float)dy * mouseSensitivity;

		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(1, 0, 0), DeltaY * DEGTORAD);

		direction = direction * Y;
	}

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(0, 1, 0), DeltaX * DEGTORAD);

		direction = X * direction;
	}

	float4x4 mat = editorCamera.camFrustrum.WorldMatrix();
	mat.SetRotatePart(direction.Normalized());
	editorCamera.camFrustrum.SetWorldMatrix(mat.Float3x4Part());

	editorCamera.camFrustrum.pos = reference + (editorCamera.camFrustrum.front * -distance);
	editorCamera.LookAt(reference);
}

void ModuleCamera3D::FreeRotation(float dt)
{
	int dx = -App->moduleInput->GetMouseXMotion();
	int dy = -App->moduleInput->GetMouseYMotion();
	/*reference -= editorCamera.camFrustrum.pos;*/

	Quat direction;
	editorCamera.camFrustrum.WorldMatrix().Decompose(float3(), direction, float3());

	if (dy != 0)
	{
		float DeltaY = (float)dy * mouseSensitivity;

		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(1, 0, 0), DeltaY * DEGTORAD);

		direction = direction * Y;
	}

	if (dx != 0)
	{
		float DeltaX = (float)dx * mouseSensitivity;

		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(0, 1, 0), DeltaX * DEGTORAD);

		direction = X * direction;
	}

	float4x4 mat = editorCamera.camFrustrum.WorldMatrix();
	mat.SetRotatePart(direction.Normalized());
	editorCamera.camFrustrum.SetWorldMatrix(mat.Float3x4Part());

	reference = editorCamera.camFrustrum.pos + editorCamera.camFrustrum.front * (reference-editorCamera.camFrustrum.pos).Length();
}

//Deprecated
/*
void ModuleCamera3D::FocusCamera(float3 center, float offset)
{

	editorCamera.LookAt(center);
	editorCamera.camFrustrum.pos = center + (((editorCamera.camFrustrum.pos - center).Normalized()) * offset);
}
*/

void ModuleCamera3D::PanCamera(float dt)
{

	//WARNING: Need to normalize movement
	int dx = -App->moduleInput->GetMouseXMotion();
	int dy = App->moduleInput->GetMouseYMotion();
	float3 aux = editorCamera.camFrustrum.pos - reference;
	float sensitivity = sqrt(aux.Length() * 0.005f) * 0.5f;/*Pow(2.0f,zoomLevel*0.075f)*0.005f;*/
	sensitivity = max(sensitivity, 0.05f);//TODO missing dt here?

	if (dx != 0)
	{
		float deltaX = dx * sensitivity;
		Move(editorCamera.camFrustrum.WorldRight() * sensitivity * deltaX);
	}

	if (dy != 0)
	{
		float deltaY = dy * sensitivity;
		Move(editorCamera.camFrustrum.up * sensitivity * deltaY);
	}




	//==========
	/*if (dx != 0 || dy != 0) 
	{
		float3 movVector((editorCamera.camFrustrum.WorldRight() * dx) + (-editorCamera.camFrustrum.up * dy));
		editorCamera.camFrustrum.pos += movVector * dt;
		reference += movVector * dt;
	}*/
}

void ModuleCamera3D::CamZoom(int addZoomAmount)
{
	zoomLevel -= addZoomAmount;
	if (zoomLevel < 1)zoomLevel = 1;	
	editorCamera.camFrustrum.pos = reference -editorCamera.camFrustrum.front * (pow(2.0f, zoomLevel * 0.1f) - 1);
}

#endif // !STANDALONE