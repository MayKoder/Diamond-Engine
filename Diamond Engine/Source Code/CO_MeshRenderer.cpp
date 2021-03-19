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
faceNormals(false), vertexNormals(false), showAABB(false), showOBB(false), drawDebugVertices(false)
{
	name = "Mesh Renderer";
	alternColor = float3::one;
}

C_MeshRenderer::~C_MeshRenderer()
{
	rootBone = nullptr;
	bonesMap.clear();

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
	if (showAABB == true)
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

	//Mesh array with transform matrix of each bone
	if (rootBone != nullptr)
	{
		//Get each bone
		for (std::map<std::string, uint>::iterator it = _mesh->bonesMap.begin(); it != _mesh->bonesMap.end(); ++it)
		{
			GameObject* bone = bonesMap[it->first];

			if (bone != nullptr)
			{
				//Calcule of Delta Matrix
				float4x4 Delta = CalculateDeltaMatrix(dynamic_cast<C_Transform*>(bone->GetComponent(Component::TYPE::TRANSFORM))->globalTransform, dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM))->globalTransform.Inverted());
				Delta = Delta * _mesh->bonesOffsets[it->second];

				//Storage of Delta Matrix (Transformation applied to each bone)
				_mesh->boneTransforms[it->second] = Delta.Transposed();
			}
		}
	}

	if (drawDebugVertices)
		DrawDebugVertices();

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

		ImGui::Button("Drop .mmh to change mesh", ImVec2(180, 40));
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
		ImGui::Checkbox("Show AABB", &showAABB);
		ImGui::Checkbox("Face Normals", &faceNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Show OBB", &showOBB);
		ImGui::Checkbox("Draw Vertices", &drawDebugVertices);

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

void C_MeshRenderer::SetRootBone(GameObject* _rootBone)
{
	if (_rootBone == nullptr) {
		LOG(LogType::L_ERROR, "Trying to assign null root bone");
		return;
	}

	rootBone = _rootBone;

	//Get all the bones
	GetBoneMapping(bonesMap);

	//Set bone Transforms array size using original bones transform array size
	_mesh->boneTransforms.resize(_mesh->bonesOffsets.size());

	if (bonesMap.size() != _mesh->bonesMap.size())
	{
		for (size_t i = 0; i < _mesh->boneTransforms.size(); i++)
		{
			_mesh->boneTransforms[i] = float4x4::identity;
		}
	}
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


float4x4 C_MeshRenderer::CalculateDeltaMatrix(float4x4 globalMat, float4x4 invertMat)
{
	float3 position;
	Quat rotation;
	float3 scale;

	float4x4 mat = globalMat;
	mat.Decompose(position, rotation, scale);
	mat = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM))->globalTransform.Inverted() * mat;
	mat.Decompose(position, rotation, scale);

	return mat;
}

void C_MeshRenderer::GetBoneMapping(std::map<std::string, GameObject*>& boneMapping)
{
	boneMapping.clear();
	std::vector<GameObject*> gameObjects;
	rootBone->CollectChilds(gameObjects);

	for (uint i = 0; i < gameObjects.size(); ++i)
	{
		boneMapping[gameObjects[i]->name] = gameObjects[i];
	}
}

void C_MeshRenderer::DrawDebugVertices()
{
	if (_mesh->boneTransforms.size() > 0)
	{
		for (uint v = 0; v < _mesh->vertices_count; ++v)
		{
			float3 vertex;
			vertex.x = _mesh->vertices[v * VERTEX_ATTRIBUTES];
			vertex.y = _mesh->vertices[v * VERTEX_ATTRIBUTES + 1];
			vertex.z = _mesh->vertices[v * VERTEX_ATTRIBUTES + 2];

			//For each set of 4 bones for bertex
			for (uint b = 0; b < 4; ++b)
			{
				//Get bone identificator and weights from arrays
				int bone_ID = _mesh->vertices[v * VERTEX_ATTRIBUTES + BONES_ID_OFFSET + b];
				float boneWeight = _mesh->vertices[v * VERTEX_ATTRIBUTES + WEIGHTS_OFFSET + b];

				//Meaning boneWeight will be 0
				if (bone_ID == -1)
					continue;

				//Transforming original mesh vertex with bone transformation matrix
				float3 vertexTransform;
				vertexTransform.x = _mesh->vertices[v * VERTEX_ATTRIBUTES];
				vertexTransform.y = _mesh->vertices[v * VERTEX_ATTRIBUTES + 1];
				vertexTransform.z = _mesh->vertices[v * VERTEX_ATTRIBUTES + 2];
				//float3 Inc = _mesh->boneTransforms[bone_ID].TransformPos(vertexTransform);

				float4 Inc = _mesh->boneTransforms[bone_ID].Transposed().Mul(float4(vertexTransform, 1.0));

				vertex.x += Inc.x * boneWeight;
				vertex.y += Inc.y * boneWeight;
				vertex.z += Inc.z * boneWeight;
			}

			glPushMatrix();
			glMultMatrixf(gameObject->transform->globalTransform.Transposed().ptr());
			glPointSize(4.0f);
			glBegin(GL_POINTS);
			glVertex3fv(vertex.ptr());
			glEnd();
			glPointSize(1.0f);
			glPopMatrix();
		}
	}
}