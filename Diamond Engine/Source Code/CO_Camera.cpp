#include "CO_Camera.h"
#include"Globals.h"
#include"DEJsonSupport.h"
#include"ImGui/imgui.h"

#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Geometry/Plane.h"
#include"MathGeoLib/include/Geometry/AABB.h"

#include"MO_Renderer3D.h"
#include"MO_Scene.h"

#include"GameObject.h"
#include"CO_Transform.h"
#include"OpenGL.h"
#include"MO_Window.h"

C_Camera::C_Camera() : Component(nullptr), fov(60.0f), cullingState(true),
msaaSamples(4), orthoSize(1.0f)
{
	name = "Camera";
	camFrustrum.type = FrustumType::PerspectiveFrustum;
	camFrustrum.nearPlaneDistance = 0.1f;
	camFrustrum.farPlaneDistance = 500.f;
	camFrustrum.front = float3::unitZ;
	camFrustrum.up = float3::unitY;

	camFrustrum.verticalFov = 60.0f * DEGTORAD;
	camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * 1.7f);

	camFrustrum.pos = float3::zero;
	orthoSize = 0.0f;
}

C_Camera::C_Camera(GameObject* _gm) : Component(_gm), fov(60.0f), cullingState(true),
msaaSamples(4), orthoSize(1.0f)
{

	name = "Camera";
	camFrustrum.type = FrustumType::PerspectiveFrustum;
	camFrustrum.nearPlaneDistance = 1;
	camFrustrum.farPlaneDistance = 100.f;
	camFrustrum.front = gameObject->transform->GetForward();
	camFrustrum.up = gameObject->transform->GetUp();

	camFrustrum.verticalFov = 60.0f * DEGTORAD;
	camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * 1.7f);
	
	camFrustrum.pos = gameObject->transform->position;
}

C_Camera::~C_Camera()
{
	msaaFBO.ClearBuffer();
	resolvedFBO.ClearBuffer();

	if (EngineExternal && EngineExternal->moduleRenderer3D->GetGameRenderTarget() == this)
		EngineExternal->moduleRenderer3D->SetGameRenderTarget(nullptr);
}

#ifndef STANDALONE
bool C_Camera::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		//ImGui::Text("FB %i, TB %i, RBO %i", framebuffer, texColorBuffer, rbo);

		ImGui::DragFloat("Near Distance: ", &camFrustrum.nearPlaneDistance, 0.1f, 0.01f, camFrustrum.farPlaneDistance);
		ImGui::DragFloat("Far Distance: ", &camFrustrum.farPlaneDistance, 0.1f, camFrustrum.nearPlaneDistance, 10000.f);

		ImGui::Separator();
		
		if (camFrustrum.type == FrustumType::PerspectiveFrustum) 
		{
			ImGui::Text("Vertical FOV: %f", camFrustrum.verticalFov);
			ImGui::Text("Horizontal FOV: %f", camFrustrum.horizontalFov);
			ImGui::Separator();
			if (ImGui::DragFloat("FOV: ", &fov, 0.1f, 1.0f, 180.f))
			{
				camFrustrum.verticalFov = fov * DEGTORAD;
				//camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * App->window->GetWindowWidth() / App->window->GetWindowHeight());
			}
		}
		else
		{
			if (ImGui::DragFloat("Size: ", &orthoSize, 0.1f, 1.0f, 180.f)) 
			{
				//camFrustrum.orthographicHeight = 1080;
				//camFrustrum.orthographicWidth = 1920;
			}
		}
		

		
		if (ImGui::BeginCombo("Frustrum Type", (camFrustrum.type == FrustumType::PerspectiveFrustum) ? "Prespective" : "Orthographic")) 
		{
			if (ImGui::Selectable("Perspective")) 
				camFrustrum.type = FrustumType::PerspectiveFrustum;

			if (ImGui::Selectable("Orthographic"))
				camFrustrum.type = FrustumType::OrthographicFrustum;

			ImGui::EndCombo();
		}

		ImGui::Text("Camera Culling: "); ImGui::SameLine();
		ImGui::Checkbox("##cameraCulling", &cullingState);

		ImGui::Text("MSAA Samples: "); ImGui::SameLine(); 
		if (ImGui::SliderInt("##msaasamp", &msaaSamples, 1, 4)) 
		{
			msaaFBO.ReGenerateBuffer(msaaFBO.texBufferSize.x, msaaFBO.texBufferSize.y, true, msaaSamples);
			resolvedFBO.ReGenerateBuffer(resolvedFBO.texBufferSize.x, resolvedFBO.texBufferSize.y, false, 0);
		}

		if(ImGui::Button("Set as Game Camera")) 
		{
			EngineExternal->moduleRenderer3D->SetGameRenderTarget(this);
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE

void C_Camera::Update()
{

	//Maybe dont update every frame?
	camFrustrum.pos = gameObject->transform->globalTransform.TranslatePart();
	camFrustrum.front = gameObject->transform->GetForward();
	camFrustrum.up = gameObject->transform->GetUp();

#ifndef STANDALONE
	float3 points[8];
	camFrustrum.GetCornerPoints(points);

	ModuleRenderer3D::DrawBox(points, float3(0.180f, 1.f, 0.937f));
#endif // !STANDALONE

}


void C_Camera::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	DEJson::WriteInt(nObj, "fType", (int)FrustumType::PerspectiveFrustum);

	DEJson::WriteFloat(nObj, "nearPlaneDist", camFrustrum.nearPlaneDistance);
	DEJson::WriteFloat(nObj, "farPlaneDist", camFrustrum.farPlaneDistance);

	DEJson::WriteFloat(nObj, "vFOV", camFrustrum.verticalFov);
	DEJson::WriteFloat(nObj, "hFOV", camFrustrum.horizontalFov);
	DEJson::WriteBool(nObj, "culling", cullingState);
}

void C_Camera::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	camFrustrum.type = (FrustumType)nObj.ReadInt("fType");

	camFrustrum.nearPlaneDistance = nObj.ReadFloat("nearPlaneDist");
	camFrustrum.farPlaneDistance = nObj.ReadFloat("farPlaneDist");

	camFrustrum.verticalFov = nObj.ReadFloat("vFOV");
	camFrustrum.horizontalFov = nObj.ReadFloat("hFOV");
	cullingState = nObj.ReadBool("culling");

	EngineExternal->moduleScene->SetGameCamera(this);
}

void C_Camera::StartDraw()
{
	EngineExternal->moduleRenderer3D->activeRenderCamera = this;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	PushCameraMatrix();

	glBindFramebuffer(GL_FRAMEBUFFER, msaaFBO.GetFrameBuffer());

	glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_DEPTH_TEST);

	//glLoadIdentity();

	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(camFrustrum.ViewMatrix().ptr());
}

void C_Camera::EndDraw()
{
	//Is this important?

	glBindTexture(GL_TEXTURE_2D, msaaFBO.GetTextureBuffer());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	/*TODO: IMPORTANT This is the MSAA resolving to screen, to resolve a MSAA FBO to a Normal FBO we need to do the same but
	add the normal fbo as the GL_DRAW_FRAMEBUFFER*/
#ifdef STANDALONE 
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO.GetFrameBuffer());
	glBlitFramebuffer(0, 0, msaaFBO.texBufferSize.x, msaaFBO.texBufferSize.y, 0, 0, msaaFBO.texBufferSize.x, msaaFBO.texBufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

#else
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFBO.GetFrameBuffer());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolvedFBO.GetFrameBuffer());
	glBlitFramebuffer(0, 0, msaaFBO.texBufferSize.x, msaaFBO.texBufferSize.y, 0, 0, resolvedFBO.texBufferSize.x, resolvedFBO.texBufferSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
#endif // !STANDALONE


	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glDisable(GL_DEPTH_TEST);
}

void C_Camera::ReGenerateBuffer(int w, int h)
{
	SetAspectRatio((float)w / (float)h);
	
	msaaFBO.ReGenerateBuffer(w, h, true, 4);
	resolvedFBO.ReGenerateBuffer(w, h, false, 0);
}

void C_Camera::PushCameraMatrix()
{
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)ProjectionMatrixOpenGL().v);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)ViewMatrixOpenGL().v);
}

void C_Camera::LookAt(const float3& Spot)
{
	/*Reference = Spot;*/
	camFrustrum.front = (Spot - camFrustrum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(camFrustrum.front).Normalized();
	camFrustrum.up = camFrustrum.front.Cross(X);
}

void C_Camera::LookAt(Frustum& frus, const float3& Spot)
{
	/*Reference = Spot;*/
	frus.front = (Spot - frus.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(frus.front).Normalized();
	frus.up = frus.front.Cross(X);
}

void C_Camera::Move(const float3& Movement)
{
	camFrustrum.pos += Movement;
}

float3 C_Camera::GetPosition()
{
	return camFrustrum.pos;
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

bool C_Camera::IsInsideFrustum(AABB& globalAABB)
{
	return (this->camFrustrum.type == FrustumType::PerspectiveFrustum) ? PrespectiveCulling(globalAABB) : OrthoCulling(globalAABB);
}
bool C_Camera::OrthoCulling(AABB& globalAABB)
{
	float3 obbPoints[8];


	float3 orthoNormals[6];
	orthoNormals[0] = -camFrustrum.front;
	orthoNormals[1] = camFrustrum.front;
	orthoNormals[2] = -camFrustrum.front.Cross(camFrustrum.up);
	orthoNormals[3] = camFrustrum.front.Cross(camFrustrum.up);
	orthoNormals[4] = camFrustrum.up;
	orthoNormals[5] = -camFrustrum.up;

	float3 planePoints[6];
	planePoints[0] = camFrustrum.pos;
	planePoints[1] = camFrustrum.pos + (camFrustrum.front * camFrustrum.farPlaneDistance);
	planePoints[2] = (camFrustrum.pos + (camFrustrum.front * (camFrustrum.farPlaneDistance / 2))) + (orthoNormals[2] * (camFrustrum.orthographicWidth / 2));
	planePoints[3] = (camFrustrum.pos + (camFrustrum.front * (camFrustrum.farPlaneDistance / 2))) + (orthoNormals[3] * (camFrustrum.orthographicWidth / 2));
	planePoints[4] = (camFrustrum.pos + (camFrustrum.front * (camFrustrum.farPlaneDistance / 2))) + (orthoNormals[4] * (camFrustrum.orthographicHeight / 2));
	planePoints[5] = (camFrustrum.pos + (camFrustrum.front * (camFrustrum.farPlaneDistance / 2))) + (orthoNormals[5] * (camFrustrum.orthographicHeight / 2));

	int totalIn = 0;
	globalAABB.GetCornerPoints(obbPoints);

	for (size_t i = 2; i < 6; i++)
	{
		int inCount = 8;
		int iPtIn = 1;

		for (size_t k = 0; k < 8; k++)
		{
			if (orthoNormals[i].Dot(obbPoints[k]) - Dot(planePoints[i], orthoNormals[i]) >= 0.f)
			{
				iPtIn = 0;
				--inCount;
			}
			if (inCount == 0)
				return false;

			totalIn += iPtIn;
		}
	}

	if (totalIn == 6)
		return true;

	return true;
}

bool C_Camera::PrespectiveCulling(AABB& globalAABB)
{
	float3 obbPoints[8];
	Plane camFrustrumumPlanes[6];

	int totalIn = 0;

	globalAABB.GetCornerPoints(obbPoints);
	camFrustrum.GetPlanes(camFrustrumumPlanes);

	for (size_t i = 0; i < 6; i++)
	{
		int inCount = 8;
		int iPtIn = 1;

		for (size_t k = 0; k < 8; k++)
		{
			//Is "IsOnPositiveSide" slow?
			if (camFrustrumumPlanes[i].IsOnPositiveSide(obbPoints[k]))
			{
				iPtIn = 0;
				--inCount;
			}
			if (inCount == 0)
				return false;

			totalIn += iPtIn;
		}
	}

	if (totalIn == 6)
		return true;

	return true;
}