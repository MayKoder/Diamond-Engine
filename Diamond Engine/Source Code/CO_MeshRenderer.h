#pragma once
#include "Component.h"

#include"MathGeoLib/include/Geometry/AABB.h"
#include"MathGeoLib/include/Geometry/OBB.h"

class ResourceMesh;

class C_MeshRenderer : public Component
{
public:
	C_MeshRenderer(GameObject* _gm);
	virtual ~C_MeshRenderer();

	void Update() override;

	void RenderMesh();

	void SaveData(JSON_Object* nObj) override;
	void LoadData(JSON_Object* nObj) override;

	bool OnEditor() override;

	bool IsInsideFrustum(Frustum* camFrustum);

	void SetRenderMesh(ResourceMesh* mesh);
	ResourceMesh* GetRenderMesh();

//private:

	//Pointer to a mesh stored at ModuleRenderer3D
	//Does not need a delete call

	AABB globalAABB;
	OBB globalOBB;

	bool faceNormals, vertexNormals, showAABB, showOBB;

private:
	ResourceMesh* _mesh;
};