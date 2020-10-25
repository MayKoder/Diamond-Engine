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
	//if (gameObject->transform != nullptr) 
	//{
	//	glPushMatrix();
	//	glMultMatrixf(gameObject->transform->transform.v[0]);
	//}

	_mesh->RenderMesh();

	//if(gameObject->transform != nullptr)
	//	glPopMatrix();
}
