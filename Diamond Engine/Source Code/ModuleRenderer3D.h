#pragma once
#include "Application.h"
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include"Primitive.h"
#include "Light.h"

#include "OpenGL.h"

#define MAX_LIGHTS 1

class Mesh 
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

public:

	uint indices_id = 0; // index in VRAM
	uint indices_count = 0;
	uint* indices = nullptr;

	//Vertices is a 1 dimension array that contains floats representing a vertex every 3 elements, vertices_count is the number of 3 pairs (0, 0, 1) would be
	//a 3 element array but vertices_count would be 1.
	uint vertices_id = 0; // unique vertex in VRAM
	uint vertices_count = 0;
	float* vertices = nullptr;

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


class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnResize(int width, int height);
	void OnGUI() override;

	void ReGenerateFrameBuffer(int w, int h);

private:

	void GetCAPS(std::string& caps);
	std::string str_CAPS;

	bool vsync;
	bool wireframe;

	bool /*depth,*/ cull, lightng, color_material, texture_2d;

	uint my_id;

	//Mesh testMesh;
	std::vector<Mesh*> test;

public:
	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};