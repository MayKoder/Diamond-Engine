//Rename as C_Mesh?
#pragma once

//WARNING: Should i remove the opengl include in .h?
#include "glmath.h"
#include "Globals.h"
#include <vector>

typedef unsigned int GLuint;

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

	void RenderMesh(GLuint textureID);

	void RenderMeshDebug(bool* vertexNormals, bool* faceNormals);

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

	//TODO: Delete this, wireframe mode should be different
	// ----------- TEMPORAL LOGIC, MUST BE DELETED ---------------//
	bool* generalWireframe;
	// ----------- ------------------------------- ---------------//

	void GenerateSphere(float radius, float sectorCount, float stackCount);

};