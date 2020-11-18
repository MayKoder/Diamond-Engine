#include "C_Camera.h"
#include"Globals.h"
#include"DEJsonSupport.h"
#include"ImGui/imgui.h"

#include"MathGeoLib/include/Math/float3.h"

#include"GameObject.h"
#include"C_Transform.h"
#include"OpenGL.h"

C_Camera::C_Camera(GameObject* _gm) : Component(_gm), imgBufferID(-1)
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
}

bool C_Camera::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		ImGui::DragFloat("Near Distance: ", &camFrustrum.nearPlaneDistance, 0.1f, 0.0f, camFrustrum.farPlaneDistance);
		ImGui::DragFloat("Far Distance: ", &camFrustrum.farPlaneDistance, 0.1f, camFrustrum.nearPlaneDistance, 10000.f);

		ImGui::DragFloat("Vertical FOV: ", &camFrustrum.verticalFov, 0.1f, 0.0f, 300.f);
		ImGui::DragFloat("Horizontal FOV: ", &camFrustrum.horizontalFov, 0.1f, 0.0f, 300.f);
		
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

	glColor3f(1.f, 0.f, 0.f);
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