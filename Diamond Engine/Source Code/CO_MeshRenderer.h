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

	void RenderMesh(bool rTex = false);

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	bool IsInsideFrustum(Frustum* camFrustum);

	void SetRenderMesh(ResourceMesh* mesh);
	ResourceMesh* GetRenderMesh();

	OBB globalOBB;
	AABB globalAABB;
	bool faceNormals, vertexNormals, showAABB, showOBB;

private:
	ResourceMesh* _mesh;
	float3 alternColor;
};