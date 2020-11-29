#include "CO_Camera.h"
#include"Globals.h"
#include"DEJsonSupport.h"
#include"ImGui/imgui.h"

#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Geometry/Plane.h"

#include"MO_Renderer3D.h"
#include"MO_Scene.h"

#include"GameObject.h"
#include"CO_Transform.h"
#include"OpenGL.h"

C_Camera::C_Camera() : Component(nullptr), framebuffer(0), texColorBuffer(0), rbo(0), fov(60.0f)
{
	name = "Camera";
	camFrustrum.type = FrustumType::PerspectiveFrustum;
	camFrustrum.nearPlaneDistance = 0.1f;
	camFrustrum.farPlaneDistance = 500.f;
	camFrustrum.front = float3::unitZ;
	camFrustrum.up = float3::unitY;

	camFrustrum.verticalFov = 60.0f * DEGTORAD;
	camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * 1.3f);

	camFrustrum.pos = float3::zero;
}

C_Camera::C_Camera(GameObject* _gm) : Component(_gm), framebuffer(0), texColorBuffer(0), rbo(0), fov(60.0f)
{
	name = "Camera";
	camFrustrum.type = FrustumType::PerspectiveFrustum;
	camFrustrum.nearPlaneDistance = 1;
	camFrustrum.farPlaneDistance = 100.f;
	camFrustrum.front = gameObject->transform->GetForward();
	camFrustrum.up = gameObject->transform->GetUp();

	camFrustrum.verticalFov = 60.0f * DEGTORAD;
	camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * 1.3f);
	
	camFrustrum.pos = gameObject->transform->position;
}

C_Camera::~C_Camera()
{
	if (framebuffer != 0)
		glDeleteFramebuffers(1, (GLuint*)&framebuffer);

	if (texColorBuffer != 0)
		glDeleteTextures(1, (GLuint*)&texColorBuffer);

	if (rbo != 0)
		glDeleteRenderbuffers(1, (GLuint*)&rbo);

	if (EngineExternal && EngineExternal->moduleRenderer3D->GetGameRenderTarget() == this)
		EngineExternal->moduleRenderer3D->SetGameRenderTarget(nullptr);
}

bool C_Camera::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		ImGui::Text("FB %i, TB %i, RBO %i", framebuffer, texColorBuffer, rbo);

		ImGui::DragFloat("Near Distance: ", &camFrustrum.nearPlaneDistance, 0.1f, 0.01f, camFrustrum.farPlaneDistance);
		ImGui::DragFloat("Far Distance: ", &camFrustrum.farPlaneDistance, 0.1f, camFrustrum.nearPlaneDistance, 10000.f);

		ImGui::Separator();
		ImGui::Text("Vertical FOV: %f", camFrustrum.verticalFov);
		ImGui::Text("Horizontal FOV: %f", camFrustrum.horizontalFov);
		ImGui::Separator();

		if (ImGui::DragFloat("FOV: ", &fov, 0.1f, 1.0f, 180.f))
		{
			camFrustrum.verticalFov = fov * DEGTORAD;
			//camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * App->window->GetWindowWidth() / App->window->GetWindowHeight());
		}
		
		if (ImGui::BeginCombo("Frustrum Type", (camFrustrum.type == FrustumType::PerspectiveFrustum) ? "Prespective" : "Orthographic")) 
		{
			if (ImGui::Selectable("Perspective")) 
				camFrustrum.type = FrustumType::PerspectiveFrustum;

			if (ImGui::Selectable("Orthographic"))
				camFrustrum.type = FrustumType::OrthographicFrustum;

			ImGui::EndCombo();
		}

		return true;
	}
	return false;
}

void C_Camera::Update()
{

	//Maybe dont update every frame?
	camFrustrum.pos = gameObject->transform->position;
	camFrustrum.front = gameObject->transform->GetForward();
	camFrustrum.up = gameObject->transform->GetUp();

	
	//glVertex3f(gameObject->transform->position.x, gameObject->transform->position.y, gameObject->transform->position.z);
	//glVertex3f(gameObject->transform->position.x + forward.x, gameObject->transform->position.y + forward.y, gameObject->transform->position.z + forward.z);
	float3 points[8];
	camFrustrum.GetCornerPoints(points);

	ModuleRenderer3D::DrawBox(points, float3(0.180f, 1.f, 0.937f));
}


void C_Camera::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	DEJson::WriteInt(nObj, "fType", (int)FrustumType::PerspectiveFrustum);

	DEJson::WriteFloat(nObj, "nearPlaneDist", camFrustrum.nearPlaneDistance);
	DEJson::WriteFloat(nObj, "farPlaneDist", camFrustrum.farPlaneDistance);

	DEJson::WriteFloat(nObj, "vFOV", camFrustrum.verticalFov);
	DEJson::WriteFloat(nObj, "hFOV", camFrustrum.horizontalFov);
}

void C_Camera::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);

	camFrustrum.type = (FrustumType)DEJson::ReadInt(nObj, "fType");

	camFrustrum.nearPlaneDistance = DEJson::ReadFloat(nObj, "nearPlaneDist");
	camFrustrum.farPlaneDistance = DEJson::ReadFloat(nObj, "farPlaneDist");

	camFrustrum.verticalFov = DEJson::ReadFloat(nObj, "vFOV");
	camFrustrum.horizontalFov = DEJson::ReadFloat(nObj, "hFOV");

	//Need to reset W_Game target canera
	EngineExternal->moduleScene->SetGameCamera(this);

}

void C_Camera::StartDraw()
{
	glEnable(GL_DEPTH_TEST);

	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)ProjectionMatrixOpenGL().v);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)ViewMatrixOpenGL().v);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_DEPTH_TEST);

	//glLoadIdentity();

	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(camFrustrum.ViewMatrix().ptr());
}

void C_Camera::EndDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);

	//glClearColor(0.05f, 0.05f, 0.05f, 1.f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void C_Camera::ReGenerateBuffer(int w, int h)
{
	SetAspectRatio((float)w / (float)h);

	if (framebuffer != 0)
		glDeleteFramebuffers(1, (GLuint*)&framebuffer);

	if (texColorBuffer != 0)
		glDeleteTextures(1, (GLuint*)&texColorBuffer);

	if (rbo != 0)
		glDeleteRenderbuffers(1, (GLuint*)&rbo);


	glGenFramebuffers(1, (GLuint*)&framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, (GLuint*)&texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, (GLuint*)&rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG(LogType::L_ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void C_Camera::LookAt(const float3& Spot)
{
	/*Reference = Spot;*/
	camFrustrum.front = (Spot - camFrustrum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(camFrustrum.front).Normalized();
	camFrustrum.up = camFrustrum.front.Cross(X);
}

//void C_Camera::Look(const float3& Position, const float3& Reference, bool RotateAroundReference)
//{
//	//	editorCamera.camFrustrum.pos = Position;
////	editorCamera.camFrustrum.pos = Reference;
////
////	Z = normalize(Position - Reference);
////	X = normalize(cross(float3(0.0f, 1.0f, 0.0f), Z));
////	Y = cross(Z, X);
////
////	if(!RotateAroundReference)
////	{
////		this->Reference = editorCamera.camFrustrum.pos;
////		editorCamera.camFrustrum.pos += Z * 0.05f;
////	}
//}

void C_Camera::Move(const float3& Movement)
{
	camFrustrum.pos += Movement;
}

float4x4 C_Camera::ViewMatrixOpenGL() const
{
	math::float4x4 mat = camFrustrum.ViewMatrix();
	return mat.Transposed();
}

float4x4 C_Camera::ProjectionMatrixOpenGL() const
{
	return camFrustrum.ProjectionMatrix().Transposed();
}

void C_Camera::SetAspectRatio(float aspectRatio)
{
	camFrustrum.horizontalFov = 2.f * atanf(tanf(camFrustrum.verticalFov * 0.5f) * aspectRatio);
}