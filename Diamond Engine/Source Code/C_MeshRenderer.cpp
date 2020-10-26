#include "C_MeshRenderer.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "C_Transform.h"
#include "GameObject.h"

#include"ImGui/imgui.h"

C_MeshRenderer::C_MeshRenderer(GameObject* _gm) : Component(_gm), _mesh(nullptr)
{
}

C_MeshRenderer::~C_MeshRenderer()
{
}

void C_MeshRenderer::Update()
{
	//Position matrix?
	C_Transform* transform = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::Type::Transform));
	if (transform != nullptr) 
	{
		glPushMatrix();
		//TODO: Save transposed floa4x4
		glMultMatrixf(transform->globalTransform.Transposed().ptr());
	}

	_mesh->RenderMesh();

	if(transform != nullptr)
		glPopMatrix();
}

void C_MeshRenderer::OnEditor()
{
	if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Image((ImTextureID)_mesh->textureID, ImVec2(128, 128));
	}
}
