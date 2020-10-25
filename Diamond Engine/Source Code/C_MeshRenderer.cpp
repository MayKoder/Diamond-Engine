#include "C_MeshRenderer.h"
#include "Mesh.h"
#include "OpenGL.h"
#include "C_Transform.h"
#include "GameObject.h"

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
		glMultMatrixf(transform->globalTransform.ptr());
	}

	_mesh->RenderMesh();

	if(transform != nullptr)
		glPopMatrix();
}
