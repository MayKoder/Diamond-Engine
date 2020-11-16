#include "C_MeshRenderer.h"
#include "Mesh.h"
#include "OpenGL.h"

#include "Application.h"
#include "ModuleRenderer3D.h"
#include"I_FileSystem.h"

#include "GameObject.h"
#include "C_Material.h"
#include "C_Transform.h"

#include "ImGui/imgui.h"
#include"DEJsonSupport.h"

C_MeshRenderer::C_MeshRenderer(GameObject* _gm) : Component(_gm), _mesh(nullptr),
faceNormals(false), vertexNormals(false)
{
	name = "Mesh Renderer";
}

C_MeshRenderer::~C_MeshRenderer()
{
}

void C_MeshRenderer::Update()
{
	EngineExternal->moduleRenderer3D->renderQueue.push_back(this);
}

void C_MeshRenderer::RenderMesh()
{

	//Position matrix?
	C_Transform* transform = gameObject->transform;
	if (transform != nullptr)
	{
		glPushMatrix();
		glMultMatrixf(transform->GetGlobalTransposed());
	}

	C_Material* material = dynamic_cast<C_Material*>(gameObject->GetComponent(Component::Type::Material));
	GLuint id = -1;

	if (material != nullptr && material->IsActive())
		id = material->GetTextureID();

	_mesh->RenderMesh(id);

	if (vertexNormals || faceNormals)
		_mesh->RenderMeshDebug(&vertexNormals, &faceNormals);

	if (transform != nullptr)
		glPopMatrix();
}

void C_MeshRenderer::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteString(nObj, "Path", _mesh->path.c_str());
}
void C_MeshRenderer::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);
	//There is no _mesh yet lol
	_mesh = new Mesh(DEJson::ReadString(nObj, "Path"));
	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;
	_mesh->LoadCustomFormat(_mesh->path.c_str());
	_mesh->GenBuffers();
	EngineExternal->moduleRenderer3D->globalMeshes.push_back(_mesh);
}

bool C_MeshRenderer::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();
		//ImGui::Image((ImTextureID)_mesh->textureID, ImVec2(128, 128));
		ImGui::Text("Vertices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->vertices_count);
		ImGui::Text("Normals: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->normals_count);
		ImGui::Text("Indices: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->indices_count);
		ImGui::Text("Texture coords: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i", _mesh->texCoords_count);

		ImGui::Spacing();
		ImGui::Text("Path: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s", _mesh->path.c_str());

		if (ImGui::Button("Select Mesh")) {

		}

		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::SameLine();
		ImGui::Checkbox("Face Normals", &faceNormals);

		return true;
	}
	return false;
}