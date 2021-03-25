#pragma once

#include "glmath.h" //Move to MathGeoLib
#include <vector>
#include<string>
#include <map>

#include"MathGeoLib/include/Geometry/AABB.h"
#include"DEResource.h"

typedef unsigned int GLuint;
typedef unsigned int uint;
class ResourceMaterial;
class C_Transform;
class Joint;

static const int VERTEX_ATTRIBUTES      = 22;  // 3 vertex + 2 texcoords + 3 normals + 3 tangents + 4 joint indices + 4 weights + 3 colors
static const int VERTEX_POSITION_OFFSET = 0;
static const int TEXCOORD_OFFSET	    = 3;
static const int NORMALS_OFFSET			= 5;
static const int TANGENTS_OFFSET		= 8;
static const int BONES_ID_OFFSET		= 11;
static const int WEIGHTS_OFFSET			= 15;
static const int COLORS_OFFSET			= 19;

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

	void OGL_GPU_Render();

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

	//Bones stuff [Remember 4 max for vertices]
	bool hasSkeleton = false;

	std::vector<float4x4> boneTransforms;
	std::map<std::string, uint> bonesMap;
	std::vector<float4x4>   bonesOffsets;

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
	void SaveBones(char** cursor);
	void LoadBones(char** cursor);
};