#include "C_Camera.h"
#include"Globals.h"
#include"DEJsonSupport.h"
#include"ImGui/imgui.h"

#include"MathGeoLib/include/Math/float3.h"

#include"GameObject.h"
#include"C_Transform.h"
#include"OpenGL.h"

C_Camera::C_Camera(GameObject* _gm) : Component(_gm), framebuffer(-1), texColorBuffer(-1), rbo(-1), fov(60.0f)
{
	name = "Camera";
	camFrustrum.type = FrustumType::PerspectiveFrustum;
	camFrustrum.nearPlaneDistance = 1;
	camFrustrum.farPlaneDistance = 10;
	camFrustrum.front = gameObject->transform->GetForward();
	camFrustrum.up = gameObject->transform->GetUp();

	camFrustrum.verticalFov = 60.0f * DEGTORAD;
	camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * 1.3f);
	
	camFrustrum.pos = gameObject->transform->position;
}

C_Camera::~C_Camera()
{
	if (framebuffer > 0)
		glDeleteFramebuffers(1, &framebuffer);

	if (texColorBuffer > 0)
		glDeleteTextures(1, &texColorBuffer);

	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);
}

bool C_Camera::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		ImGui::DragFloat("Near Distance: ", &camFrustrum.nearPlaneDistance, 0.1f, 0.01f, camFrustrum.farPlaneDistance);
		ImGui::DragFloat("Far Distance: ", &camFrustrum.farPlaneDistance, 0.1f, camFrustrum.nearPlaneDistance, 10000.f);

		ImGui::DragFloat("Vertical FOV: ", &camFrustrum.verticalFov, 0.1f, 0.0f, 300.f);
		ImGui::DragFloat("Horizontal FOV: ", &camFrustrum.horizontalFov, 0.1f, 0.0f, 300.f);

		if (ImGui::DragFloat("FOV: ", &fov, 0.1f, 0.0f, 180.f))
		{
			camFrustrum.verticalFov = fov * DEGTORAD;
			//camFrustrum.horizontalFov = 2.0f * atanf(tanf(camFrustrum.verticalFov / 2.0f) * App->window->GetWindowWidth() / App->window->GetWindowHeight());
		}
		
		//if (ImGui::BeginCombo("Frustrum Type", (camFrustrum.type == FrustumType::PerspectiveFrustum) ? "Prespective" : "Orthographic")) 
		//{
		//	for (size_t i = 1; i <= FrustumType::PerspectiveFrustum; i++)
		//	{
		//		if (ImGui::Selectable(((FrustumType)i == FrustumType::PerspectiveFrustum) ? "Prespective" : "Orthographic")) 
		//		{
		//			camFrustrum.type == (FrustumType)i;
		//		}
		//	}
		//	ImGui::EndCombo();
		//}

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

	//Temporal camera debug
	float3 forward = gameObject->transform->GetForward() * 10;

	glColor3f(0.501f, 1.f, 0.988f);
	glPointSize(10.f);
	glBegin(GL_LINES);

	//glVertex3f(gameObject->transform->position.x, gameObject->transform->position.y, gameObject->transform->position.z);
	//glVertex3f(gameObject->transform->position.x + forward.x, gameObject->transform->position.y + forward.y, gameObject->transform->position.z + forward.z);
	float3 points[8];
	camFrustrum.GetCornerPoints(points);

	//Draw plane
	glVertex3fv(&points[0].x);
	glVertex3fv(&points[4].x);
	glVertex3fv(&points[4].x);
	glVertex3fv(&points[6].x);
	glVertex3fv(&points[6].x);
	glVertex3fv(&points[2].x);
	glVertex3fv(&points[2].x);
	glVertex3fv(&points[0].x);

	glVertex3fv(&points[4].x);
	glVertex3fv(&points[5].x);
	glVertex3fv(&points[5].x);
	glVertex3fv(&points[7].x);
	glVertex3fv(&points[7].x);
	glVertex3fv(&points[6].x);
	glVertex3fv(&points[6].x);
	glVertex3fv(&points[4].x);

	glVertex3fv(&points[5].x);
	glVertex3fv(&points[1].x);
	glVertex3fv(&points[1].x);
	glVertex3fv(&points[3].x);
	glVertex3fv(&points[3].x);
	glVertex3fv(&points[7].x);
	glVertex3fv(&points[7].x);
	glVertex3fv(&points[5].x);

	glVertex3fv(&points[1].x);
	glVertex3fv(&points[3].x);
	glVertex3fv(&points[3].x);
	glVertex3fv(&points[2].x);
	glVertex3fv(&points[2].x);
	glVertex3fv(&points[0].x);
	glVertex3fv(&points[0].x);
	glVertex3fv(&points[1].x);


	glEnd();
	glPointSize(1.f);
	glColor3f(1.f, 1.f, 1.f);
}


void C_Camera::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	//DEJson::WriteString(nObj, "Path", matTexture->path.c_str());
	//DEJson::WriteString(nObj, "Name", matTexture->name.c_str());
	//TODO: Call texture importer and load data
}

void C_Camera::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);
	//There is no _mesh yet lol

	//int w, h;
	//w = h = 0;
	//std::string texPath = json_object_get_string(nObj, "Path");
	//std::string texName = json_object_get_string(nObj, "Name");

	//char* buffer = nullptr;
	//int size = FileSystem::LoadToBuffer(texPath.c_str(), &buffer);
	//GLuint id = TextureImporter::CustomLoadImage(buffer, size, &w, &h);


	//matTexture = new Texture(id, w, h, texName, texPath);


	//EngineExternal->moduleRenderer3D->globalTextures.push_back(matTexture);
	//RELEASE_ARRAY(buffer);
}

void C_Camera::StartDraw()
{
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((GLfloat*)GetOpenGLProjectionMatrix().v);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((GLfloat*)GetOpenGLViewMatrix().v);

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

	glClearColor(0.05f, 0.05f, 0.05f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void C_Camera::ReGenerateBuffer(int w, int h)
{
	if (framebuffer > 0)
		glDeleteFramebuffers(1, &framebuffer);

	if (texColorBuffer > 0)
		glDeleteTextures(1, &texColorBuffer);

	if (rbo > 0)
		glDeleteRenderbuffers(1, &rbo);


	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glGenTextures(1, &texColorBuffer);
	glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	// attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG(LogType::L_ERROR, "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float4x4 C_Camera::GetOpenGLViewMatrix() const
{
	math::float4x4 mat = camFrustrum.ViewMatrix();
	return mat.Transposed();
}

float4x4 C_Camera::GetOpenGLProjectionMatrix() const
{
	return camFrustrum.ProjectionMatrix().Transposed();
}

void C_Camera::SetAspectRatio(float aspectRatio)
{
	camFrustrum.horizontalFov = 2.f * atanf(tanf(camFrustrum.verticalFov * 0.5f) * aspectRatio);
}