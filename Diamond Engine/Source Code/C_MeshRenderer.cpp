#include "C_MeshRenderer.h"
#include "Mesh.h"
#include "OpenGL.h"

#include "GameObject.h"
#include "C_Material.h"
#include "C_Transform.h"

#include "ImGui/imgui.h"

C_MeshRenderer::C_MeshRenderer(GameObject* _gm) : Component(_gm), _mesh(nullptr),
faceNormals(false), vertexNormals(false)
{
}

C_MeshRenderer::~C_MeshRenderer()
{
}

void C_MeshRenderer::Update()
{
	//Position matrix?
	C_Transform* transform = gameObject->transform;
	if (transform != nullptr) 
	{
		glPushMatrix();
		//TODO: Save transposed floa4x4
		glMultMatrixf(transform->GetGlobalTransposed());
	}

	C_Material* material = dynamic_cast<C_Material*>(gameObject->GetComponent(Component::Type::Material));

	_mesh->RenderMesh(material->GetTextureID());

	if (vertexNormals || faceNormals)
		_mesh->RenderMeshDebug(&vertexNormals, &faceNormals);

	if(transform != nullptr)
		glPopMatrix();
}

void C_MeshRenderer::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//ImGui::Image((ImTextureID)_mesh->textureID, ImVec2(128, 128));

		ImGui::Checkbox("Vertex Normals", &vertexNormals);
		ImGui::Checkbox("Face Normals", &faceNormals);

	}
}
