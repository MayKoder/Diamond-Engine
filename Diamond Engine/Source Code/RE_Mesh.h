#pragma once

#include "glmath.h" //Move to MathGeoLib
#include <vector>
#include<string>

#include"MathGeoLib/include/Geometry/AABB.h"
#include"DEResource.h"

typedef unsigned int GLuint;
typedef unsigned int uint;
class ResourceMaterial;
class C_Transform;

#define VERTEX_ATTRIBUTES 11

class ResourceMesh  : public Resource
{

public:
	ResourceMesh(unsigned int _uid);
	~ResourceMesh();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void RenderMesh(GLuint textureID, float3 color, bool renderTexture = false, ResourceMaterial* shader = nullptr, C_Transform* _transform = nullptr);
	void RenderMeshDebug(bool* vertexNormals, bool* faceNormals, const float*);

	vec3 GetVectorFromIndex(float* startValue);

public:

	uint indices_id = 0; // index in VRAM
	uint indices_count = 0;
	uint* indices = nullptr;

	//Vertices is a 1 dimension array that contains floats representing a vertex every 3 elements, vertices_count is the number of 3 pairs (0, 0, 1) would be
	//a 3 element array but vertices_count would be 1.
	uint vertices_id = 0; // unique vertex in VRAM
	uint vertices_count = 0;
	float* vertices = nullptr;

	AABB localAABB;

	//TODO: Delete this, wireframe mode should be different
	// ----------- TEMPORAL LOGIC, MUST BE DELETED ---------------//
	bool* generalWireframe;
	// ----------- ------------------------------- ---------------//

	uint VBO;
	uint VAO;
	uint EBO;

	//TODO: Move this to file system
	const char* SaveCustomFormat(uint& retSize);
	void LoadCustomFormat(const char*);
};