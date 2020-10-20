//Rename as C_Mesh?
#pragma once

//WARNING: Should i remove the opengl include in .h?
#include "glmath.h"
#include "Globals.h"

struct Mesh
{

public:
	Mesh();
	~Mesh();

	void SetAsCube();
	void SetAsPyramid();
	void SetAsSphere();
	void SetAsCylinder();

	void GenBuffers();

	void RenderMesh();

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

	uint normalbuffer_id = 0;
	uint normals_count = 0;
	float* normals = nullptr;

	uint texCoords_id = 0;
	uint texCoords_count = 0;
	float* texCoords = nullptr;

	uint textureID = 0;

	//TODO: Delete this, wireframe mode should be different
	// ----------- TEMPORAL LOGIC, MUST BE DELETED ---------------//
	bool* generalWireframe;
	// ----------- ------------------------------- ---------------//

	//static void push_indices(uint* indices, int sectors, int r, int s)
	//{
	//	int curRow = r * sectors;
	//	int nextRow = (r + 1) * sectors;

	//	indices.push_back(curRow + s);
	//	indices.push_back(nextRow + s);
	//	indices.push_back(nextRow + (s + 1));

	//	indices.push_back(curRow + s);
	//	indices.push_back(nextRow + (s + 1));
	//	indices.push_back(curRow + (s + 1));
	//}

	//static void createSphere(float* vertices, uint* indices, /*std::vector<vec2>& texcoords,*/ float radius, unsigned int rings, unsigned int sectors)
	//{
	//	float const R = 1. / (float)(rings - 1);
	//	float const S = 1. / (float)(sectors - 1);

	//	for (int r = 0; r < rings; ++r) {
	//		for (int s = 0; s <= sectors; ++s) {
	//			float const y = sin(-M_PI_2 + M_PI * r * R);
	//			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
	//			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

	//			//texcoords.push_back(vec2(s * S, r * R));
	//			vertices.push_back(x * radius);
	//			vertices.push_back(y * radius);
	//			vertices.push_back(z * radius);
	//			
	//			push_indices(indices, sectors, r, s);
	//		}
	//	}
	//}

};