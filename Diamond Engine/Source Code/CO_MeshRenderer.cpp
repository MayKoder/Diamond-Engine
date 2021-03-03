#include "CO_MeshRenderer.h"
#include "RE_Mesh.h"
#include "OpenGL.h"

#include "Application.h"
#include "MO_Renderer3D.h"
#include"MO_ResourceManager.h"
#include"RE_Material.h"
#include"RE_Shader.h"
#include "IM_FileSystem.h"

#include "GameObject.h"
#include "CO_Material.h"
#include "CO_Transform.h"
#include"CO_Camera.h"

#include "ImGui/imgui.h"
#include"MO_Scene.h"

#include"DEJsonSupport.h"

#include"MathGeoLib/include/Geometry/Frustum.h"
#include"MathGeoLib/include/Geometry/Plane.h"

C_MeshRenderer::C_MeshRenderer(GameObject* _gm) : Component(_gm), _mesh(nullptr),
faceNormals(false), vertexNormals(false), showAABB(false), showOBB(false)
{
	name = "Mesh Renderer";
	alternColor = float3::one;
}

C_MeshRenderer::~C_MeshRenderer()
{
	if (_mesh != nullptr) 
	{
		EngineExternal->moduleResources->UnloadResource(_mesh->GetUID());
		_mesh = nullptr;
	}
}

void C_MeshRenderer::Update()
{
	if (EngineExternal->moduleRenderer3D->GetGameRenderTarget() != nullptr && EngineExternal->moduleRenderer3D->GetGameRenderTarget()->cullingState == true && !IsInsideFrustum(&EngineExternal->moduleRenderer3D->GetGameRenderTarget()->camFrustrum)) 
		return;
	
	EngineExternal->moduleRenderer3D->renderQueue.push_back(this);

#ifndef STANDALONE
	if (showAABB ==true) 
	{

		float3 points[8];
		globalAABB.GetCornerPoints(points);
		ModuleRenderer3D::DrawBox(points, float3(0.2f, 1.f, 0.101f));
	}

	if (showOBB == true) 
	{

		float3 points[8];
		globalOBB.GetCornerPoints(points);
		ModuleRenderer3D::DrawBox(points);
	}
#endif // !STANDALONE

}

void C_MeshRenderer::RenderMesh(bool rTex)
{
	if (_mesh == nullptr)
		return;


	C_Transform* transform = gameObject->transform;

	//TODO IMPORTANT: Optimize this, save this pointer or something
	C_Material* material = dynamic_cast<C_Material*>(gameObject->GetComponent(Component::TYPE::MATERIAL));
	GLuint id = 0;

	if (material != nullptr && material->IsActive())
		id = material->GetTextureID();

	_mesh->RenderMesh(id, alternColor, rTex, (material && material->material != nullptr) ? material->material : EngineExternal->moduleScene->defaultMaterial, transform);

	if (vertexNormals || faceNormals)
		_mesh->RenderMeshDebug(&vertexNormals, &faceNormals, transform->GetGlobalTransposed());

}

void C_MeshRenderer::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	if (_mesh) //TODO: I don't think this is a good idea
	{
		DEJson::WriteString(nObj, "Path", _mesh->GetLibraryPath());
		DEJson::WriteInt(nObj, "UID", _mesh->GetUID());
	}

	DEJson::WriteVector3(nObj, "alternColor", &alternColor.x);
}


void C_MeshRenderer::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);


	SetRenderMesh(dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("UID"), nObj.ReadString("Path"))));

	alternColor = nObj.ReadVector3("alternColor");

	if (_mesh == nullptr)
		return;

	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;

	gameObject->transform->UpdateBoxes();
}

#ifndef STANDALONE
bool C_MeshRenderer::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (_mesh != nullptr) 
		{
			//ImGui::Image((ImTextureID)_mesh->textureID, ImVec2(128, 128));
			ImGui::Text("Vertices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->vertices_count);
			ImGui::Text("Indices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->indices_count);

			ImGui::Spacing();
			ImGui::Text("Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", _mesh->GetLibraryPath());
		}

		ImGui::Button("Drop .mmh to change mesh", ImVec2(200, 50));
		//TODO: Maybe move this into a function?
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MESH"))
			{
				std::string* libraryDrop = (std::string*)payload->Data;

				if (_mesh != nullptr) {
					EngineExternal->moduleResources->UnloadResource(_mesh->GetUID());
					_mesh = nullptr;
				}

				//TODO: Almost done come on TEMPORAL: This is the only way until fbx's .meta files
				//Store an array of meshes to reference
				std::string stID = "";
				FileSystem::GetFileName(libraryDrop->c_str(), stID, false);

				//ATOI is C++11 only, maybe not a good idea to use it
				int UID = std::atoi(stID.c_str());
				SetRenderMesh(dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->RequestResource(UID, libraryDrop->c_str())));
				LOG(LogType::L_WARNING, "Mesh %s changed", (*libraryDrop).c_str());
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &faceNormals);

		ImGui::Checkbox("Show AABB", &showAABB);
		ImGui::SameLine();
		ImGui::Checkbox("Show OBB", &showOBB);

		ImGui::ColorPicker3("No texture color: ", &alternColor.x);

		return true;
	}
	return false;
}
#endif // !STANDALONE

bool C_MeshRenderer::IsInsideFrustum(Frustum* camFrustum)
{
	float3 obbPoints[8];
	Plane frustumPlanes[6];

	int totalIn = 0;

	globalAABB.GetCornerPoints(obbPoints);
	camFrustum->GetPlanes(frustumPlanes);

	for (size_t i = 0; i < 6; i++)
	{
		int inCount = 8;
		int iPtIn = 1;

		for (size_t k = 0; k < 8; k++)
		{
			//Is "IsOnPositiveSide" slow?
			if (frustumPlanes[i].IsOnPositiveSide(obbPoints[k])) 
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

void C_MeshRenderer::SetRenderMesh(ResourceMesh* mesh)
{ 
	_mesh = mesh;
	//_mesh->LoadCustomFormat(_mesh->GetLibraryPath());

	if (mesh == nullptr)
		return;

	globalOBB = _mesh->localAABB;
	globalOBB.Transform(gameObject->transform->globalTransform);

	// Generate global AABB
	globalAABB.SetNegativeInfinity();
	globalAABB.Enclose(globalOBB);

	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;
}

ResourceMesh* C_MeshRenderer::GetRenderMesh()
{
	return _mesh;
}
